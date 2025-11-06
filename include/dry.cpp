#include "main.h"

// Declarações de função (Forward declarations) para resolver dependências
void heater_on();
void heater_off();
void fan_on();
void fan_off();
bool get_fan();
#ifdef ENABLE_SERVO
void servo_on();
void servo_off();
#else
void servo_on() {}
void servo_off() {}
#endif

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

// --- Perfis de Secagem ---
const DryProfile dry_profiles[] = {
    {"ABS Default",  // nome
     80.0,           // temperatura_alvo
     5.0,            // histerese_temp
     360,            // tempo_total_min
     30,             // tempo_preaquecimento_min
     20,             // ciclo_agitacao_min
     60,             // duracao_agitacao_seg
     30,             // ciclo_exaustao_min
     120,            // duracao_exaustao_seg
     20.0},          // umidade_exaustao

    {"PLA Default",  // nome
     50.0,           // temperatura_alvo
     2.0,            // histerese_temp
     240,            // tempo_total_min (4h)
     15,             // tempo_preaquecimento_min
     30,             // ciclo_agitacao_min
     60,             // duracao_agitacao_seg
     60,             // ciclo_exaustao_min
     120,            // duracao_exaustao_seg
     40.0},          // umidade_exaustao

    {"PETG Default",  // nome
     65.0,            // temperatura_alvo
     3.0,             // histerese_temp
     300,             // tempo_total_min (5h)
     20,              // tempo_preaquecimento_min
     30,              // ciclo_agitacao_min
     60,              // duracao_agitacao_seg
     45,              // ciclo_exaustao_min
     120,             // duracao_exaustao_seg
     30.0},           // umidade_exaustao

    {"TPU Default",  // nome
     55.0,           // temperatura_alvo
     3.0,            // histerese_temp
     480,            // tempo_total_min (8h)
     20,             // tempo_preaquecimento_min
     45,             // ciclo_agitacao_min
     60,             // duracao_agitacao_seg
     60,             // ciclo_exaustao_min
     120,            // duracao_exaustao_seg
     35.0},          // umidade_exaustao

    {"Nylon (PA) Default",  // nome
     75.0,                  // temperatura_alvo
     5.0,                   // histerese_temp
     720,                   // tempo_total_min (12h)
     30,                    // tempo_preaquecimento_min
     30,                    // ciclo_agitacao_min
     60,                    // duracao_agitacao_seg
     30,                    // ciclo_exaustao_min
     180,                   // duracao_exaustao_seg
     15.0}                  // umidade_exaustao
};

const int num_profiles = sizeof(dry_profiles) / sizeof(DryProfile);
int current_profile_index = 0;  // Default to the first profile (ABS)

// Perfil ativo atual
DryProfile current_profile = dry_profiles[0];

// --- Controle de Estado do Ciclo ---
enum class DryCycleState { IDLE, PREHEATING, DRYING, DONE };
DryCycleState current_dry_state = DryCycleState::IDLE;

Ticker dry_timer;

// Time counters (in seconds). `volatile` is crucial as they are modified in an
// ISR.
volatile long remaining_time_sec = 0;
volatile long preheat_time_sec = 0;
volatile long next_agitation_sec = 0;
volatile long agitation_duration_sec = 0;
volatile long next_exhaust_sec = 0;
volatile long exhaust_duration_sec = 0;

void __dry_timer_callback() {
  // This runs every second and decrements all active counters.
  if (remaining_time_sec > 0) remaining_time_sec--;
  if (preheat_time_sec > 0) preheat_time_sec--;
  if (next_agitation_sec > 0) next_agitation_sec--;
  if (agitation_duration_sec > 0) agitation_duration_sec--;
  if (next_exhaust_sec > 0) next_exhaust_sec--;
  if (exhaust_duration_sec > 0) exhaust_duration_sec--;
}

// --- Helper functions for cycle logic ---

bool __is_exhaust_active() { return exhaust_duration_sec > 0; }
bool __is_exhaust_finished() { return exhaust_duration_sec <= 0; }
bool __is_time_for_exhaust() { return next_exhaust_sec <= 0; }

bool __is_agitation_active() { return agitation_duration_sec > 0; }
bool __is_agitation_finished() { return agitation_duration_sec <= 0; }
bool __is_time_for_agitation() { return next_agitation_sec <= 0; }

void __start_exhaust_cycle() {
  exhaust_duration_sec = current_profile.duracao_exaustao_seg;
}
void __reset_exhaust_timer() {
  next_exhaust_sec = current_profile.ciclo_exaustao_min * 60;
}

void __start_agitation_cycle() {
  agitation_duration_sec = current_profile.duracao_agitacao_seg;
}
void __reset_agitation_timer() {
  next_agitation_sec = current_profile.ciclo_agitacao_min * 60;
}

void stop_dry_cycle() {
  heater_off();
  fan_off();
  servo_off();
  dry_timer.detach();  // Stop the timer
  current_dry_state = DryCycleState::IDLE;
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

  // Initialize all counters from the profile
  remaining_time_sec = current_profile.tempo_total_min * 60;
  preheat_time_sec = current_profile.tempo_preaquecimento_min * 60;
  __reset_exhaust_timer();
  __reset_agitation_timer();
  current_dry_state = DryCycleState::PREHEATING;
  dry_timer.attach(1, __dry_timer_callback);  // Start the 1-second timer

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
  // Calculate from seconds
  unsigned int hours = remaining_time_sec / 3600;
  unsigned int minutes = (remaining_time_sec % 3600) / 60;
  char buf[12];  // Buffer maior para segurança (HH:MM\0)
  snprintf(buf, sizeof(buf), "%02u:%02u", hours, minutes);
  return String(buf);
}

String get_dry_cycle_state_str() {
  switch (current_dry_state) {
    case DryCycleState::IDLE:
      return "Idle";
    case DryCycleState::PREHEATING:
      return "Heating";
    case DryCycleState::DRYING:
      return "Drying";
    case DryCycleState::DONE:
      return "Done";
    default:
      return "Unknown";
  }
}

const char* get_current_profile_name() { return current_profile.nome; }

void next_profile() {
  // Can't change profile while a cycle is running
  if (current_dry_state != DryCycleState::IDLE) {
    return;
  }
  current_profile_index = (current_profile_index + 1) % num_profiles;
  current_profile = dry_profiles[current_profile_index];
#ifdef DEBUG
  Serial.println("! Profile changed to: " + String(get_current_profile_name()));
#endif
}

void handle_dry() {
  // A máquina de estados só executa se não estiver ociosa (IDLE)
  if (current_dry_state != DryCycleState::IDLE) {
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
        // Check if preheating time is over
        if (preheat_time_sec <= 0) {
          current_dry_state = DryCycleState::DRYING;
#ifdef DEBUG
          Serial.println("! Fase: Secagem");
#endif
        }
        break;
      }

      case DryCycleState::DRYING: {
        // Check if the total drying time is over
        if (remaining_time_sec <= 0) {
          current_dry_state = DryCycleState::DONE;
#ifdef DEBUG
          Serial.println("! Ciclo de secagem concluído.");
#endif
          stop_dry_cycle();  // This will also turn off heater/fan and timer
          return;            // Finaliza a execução
        }

        // --- Lógica de Exaustão ---
        if (__is_exhaust_active()) {
          // Exhaust cycle is active, do nothing until it finishes
        } else if (__is_time_for_exhaust()) {
          // Time to start a new exhaust cycle, if agitation is not running
          if (__is_agitation_finished()) {
#ifdef DEBUG
            Serial.println("! Iniciando ciclo de exaustão.");
#endif
            servo_on();
            fan_on();
            __start_exhaust_cycle();
#ifdef DEBUG
            Serial.println("  -> Resetando timer de exaustão.");
#endif
            __reset_exhaust_timer();
          }
        }

        // --- Lógica de Agitação ---
        if (__is_agitation_active()) {
          // Agitation cycle is active, do nothing until it finishes
        } else if (__is_time_for_agitation()) {
          // Time to start a new agitation cycle, if exhaust is not running
          if (__is_exhaust_finished()) {
#ifdef DEBUG
            Serial.println("! Iniciando ciclo de agitação.");
#endif
            fan_on();
            __start_agitation_cycle();
#ifdef DEBUG
            Serial.println("  -> Resetando timer de agitação.");
#endif
            __reset_agitation_timer();
          }
        }

        // Turn fan off if both cycles just finished
        if (get_fan() && __is_agitation_finished() && __is_exhaust_finished()) {
#ifdef DEBUG
          Serial.println("! Fim de ciclo(s) de ventilação. Desligando FAN.");
#endif
          servo_off();
          fan_off();
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
}

void init_dry() {
#ifdef DEBUG
  Serial.println("* DRYER OK");
#endif
}
