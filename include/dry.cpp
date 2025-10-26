#include "main.h"

// Declarações de função (Forward declarations) para resolver dependências
void heater_on();
void heater_off();
void fan_on();
void fan_off();

// --- Parâmetros do Ciclo de Secagem ---
// No futuro, estes valores serão carregados de um arquivo de perfil.
struct DryProfile {
  const char* nome;
  float temperatura_alvo;
  float histerese_temp;
  unsigned long tempo_total_min;
  unsigned long tempo_preaquecimento_min;
  unsigned long ciclo_agitacao_min;
  unsigned long duracao_agitacao_seg;
  unsigned long ciclo_exaustao_min;
  unsigned long duracao_exaustao_seg;
  float umidade_exaustao;
};

// Perfil fixo para PLA (conforme discutido)
// // Perfil fixo para PLA (referência)
// DryProfile current_profile = {
//     "PLA Padrão",  // nome
//     50.0,          // temperatura_alvo
//     2.0,           // histerese_temp
//     240,           // tempo_total_min
//     15,            // tempo_preaquecimento_min
//     30,            // ciclo_agitacao_min
//     60,            // duracao_agitacao_seg
//     60,            // ciclo_exaustao_min
//     120,           // duracao_exaustao_seg
//     40.0           // umidade_exaustao
// };

// Perfil fixo para ABS (para testes)
DryProfile current_profile = {
    "ABS Padrão",  // nome
    80.0,          // temperatura_alvo
    5.0,           // histerese_temp
    360,           // tempo_total_min
    30,            // tempo_preaquecimento_min
    20,            // ciclo_agitacao_min
    60,            // duracao_agitacao_seg
    40,            // ciclo_exaustao_min
    180,           // duracao_exaustao_seg
    20.0           // umidade_exaustao
};

// --- Controle de Estado do Ciclo ---
enum class DryCycleState { IDLE, PREHEATING, DRYING, DONE };
DryCycleState current_dry_state = DryCycleState::IDLE;

unsigned long remaining_time_min = 0;
unsigned long cycle_start_time_ms = 0;
unsigned long last_event_time_ms = 0;

void stop_dry_cycle() {
  heater_off();
  fan_off();
  current_dry_state = DryCycleState::IDLE;
  remaining_time_min = 0;
#ifdef DEBUG
  Serial.println("! Ciclo de secagem interrompido.");
#endif
}

void start_dry_cycle() {
  if (current_dry_state != DryCycleState::IDLE) {
#ifdef DEBUG
    Serial.println("! Um ciclo de secagem já está em andamento.");
#endif
    return;
  }

  cycle_start_time_ms = millis();
  last_event_time_ms = cycle_start_time_ms;
  remaining_time_min = current_profile.tempo_total_min;
  current_dry_state = DryCycleState::PREHEATING;

#ifdef DEBUG
  Serial.println("! Iniciando ciclo de secagem para " +
                 String(current_profile.nome));
  Serial.println("! Fase: Pré-aquecimento");
#endif
}

DryCycleState get_dry_cycle_state() { return current_dry_state; }

String get_remaining_time_str() {
  if (current_dry_state == DryCycleState::IDLE ||
      current_dry_state == DryCycleState::DONE) {
    return "--:--";
  }
  unsigned int hours = remaining_time_min / 60;
  unsigned int minutes = remaining_time_min % 60;
  char buf[12];  // Buffer maior para segurança (HH:MM\0)
  snprintf(buf, sizeof(buf), "%02u:%02u", hours, minutes);
  return String(buf);
}

String get_dry_cycle_state_str() {
  switch (current_dry_state) {
    case DryCycleState::IDLE:
      return "Ocioso";
    case DryCycleState::PREHEATING:
      return "Pré-aquecendo";
    case DryCycleState::DRYING:
      return "Secando";
    case DryCycleState::DONE:
      return "Concluído";
    default:
      return "Desconhecido";
  }
}

void handle_dry() {
  // A máquina de estados só executa se não estiver ociosa (IDLE)

  unsigned long elapsed_time_ms = millis() - cycle_start_time_ms;
  unsigned long elapsed_minutes = elapsed_time_ms / 60000;

  // Atualiza o tempo restante
  if (elapsed_minutes < current_profile.tempo_total_min) {
    remaining_time_min = current_profile.tempo_total_min - elapsed_minutes;
  } else {
    remaining_time_min = 0;
  }

  // 1. Controle de Temperatura (comum a todas as fases ativas)
  float current_temp = get_temperature();
  if (current_temp <
      (current_profile.temperatura_alvo - current_profile.histerese_temp)) {
    heater_on();
  } else if (current_temp >= current_profile.temperatura_alvo) {
    heater_off();
  }

  // 2. Lógica de transição de estados e eventos
  switch (current_dry_state) {
    case DryCycleState::PREHEATING: {
      // Verifica se o tempo de pré-aquecimento terminou
      if (elapsed_minutes >= current_profile.tempo_preaquecimento_min) {
        current_dry_state = DryCycleState::DRYING;
        last_event_time_ms = millis();  // Reseta o timer de eventos
#ifdef DEBUG
        Serial.println("! Fase: Secagem");
#endif
      }
      break;
    }

    case DryCycleState::DRYING: {
      // Verifica se o tempo total de secagem terminou
      if (elapsed_minutes >= current_profile.tempo_total_min) {
        current_dry_state = DryCycleState::DONE;
        stop_dry_cycle();
#ifdef DEBUG
        Serial.println("! Ciclo de secagem concluído.");
#endif
        return;  // Finaliza a execução
      }

      // Lógica de ventilação/exaustão baseada em tempo
      // (Esta lógica é simplificada. Para ciclos simultâneos, seria mais
      // complexo)
      unsigned long event_elapsed_ms = millis() - last_event_time_ms;

      // Exaustão (prioridade maior)
      if (event_elapsed_ms >= (current_profile.ciclo_exaustao_min * 60000)) {
        fan_on();  // Futuramente, aqui também abriria a ventilação externa
#ifdef DEBUG
        Serial.println("! Iniciando ciclo de exaustão.");
#endif
        // Mantém ligado pela duração e depois desliga
        if (event_elapsed_ms >=
            (current_profile.ciclo_exaustao_min * 60000) +
                (current_profile.duracao_exaustao_seg * 1000)) {
          fan_off();
          last_event_time_ms = millis();  // Reseta para o próximo ciclo
        }
      }
      // Agitação
      else if (event_elapsed_ms >=
               (current_profile.ciclo_agitacao_min * 60000)) {
        fan_on();
#ifdef DEBUG
        Serial.println("! Iniciando ciclo de agitação.");
#endif
        // Mantém ligado pela duração e depois desliga
        if (event_elapsed_ms >=
            (current_profile.ciclo_agitacao_min * 60000) +
                (current_profile.duracao_agitacao_seg * 1000)) {
          fan_off();
          last_event_time_ms = millis();  // Reseta para o próximo ciclo
        }
      }
      break;
    }

    case DryCycleState::IDLE:
    case DryCycleState::DONE:
    default:
      // Não faz nada nestes estados
      break;
  }
}

void init_dry() {
#ifdef DEBUG
  Serial.println("* DRYER OK");
#endif
}
