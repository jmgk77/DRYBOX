#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura para Temperatura e Umidade
typedef struct {
    float temperature;
    float humidity;
} TH_INFO;

int main(int argc, char *argv[]) {
    // 1. Verifica o número de argumentos
    if (argc != 3) {
        // argc deve ser 3: [0] nome do programa, [1] arquivo binário, [2] arquivo CSV
        fprintf(stderr, "Usage: %s <INPUT_BINARY_FILE> <OUTPUT_CSV_FILE>\n", argv[0]);
        fprintf(stderr, "Example: %s th_data.bin th_output.csv\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Variáveis de nome de arquivo
    const char *binary_filename = argv[1];
    const char *csv_filename = argv[2];

    FILE *bin_file = NULL;
    FILE *csv_file = NULL;
    TH_INFO current_data;
    size_t records_read = 0;

    // 2. Abre o arquivo binário para leitura
    // 'rb' (read binary)
    bin_file = fopen(binary_filename, "rb");
    if (bin_file == NULL) {
        perror("Error opening input binary file");
        // Mostra o nome do arquivo que causou o erro
        fprintf(stderr, "Could not open: %s\n", binary_filename);
        return EXIT_FAILURE;
    }

    // 3. Abre o arquivo CSV para escrita
    // 'w' (write - text mode)
    csv_file = fopen(csv_filename, "w");
    if (csv_file == NULL) {
        perror("Error opening output CSV file for writing");
        fclose(bin_file); // Garante que o arquivo binário seja fechado
        return EXIT_FAILURE;
    }

    // Escreve o cabeçalho no arquivo CSV
    fprintf(csv_file, "Temperature (C),Humidity (%%)\n");

    // 4. Lê o arquivo binário, estrutura por estrutura, e grava no CSV
    printf("Processing file: %s -> %s\n", binary_filename, csv_filename);
    
    // O loop continua enquanto fread retorna 1 (um registro lido com sucesso)
    while (fread(&current_data, sizeof(TH_INFO), 1, bin_file) == 1) {
        // Grava os dados. Usamos %.2f para garantir 2 casas decimais.
        fprintf(csv_file, "%.2f,%.2f\n", 
                current_data.temperature, 
                current_data.humidity);
        records_read++;
    }

    // 5. Verifica se houve erro na leitura (diferente de EOF)
    if (ferror(bin_file)) {
        perror("Error while reading binary file");
    }

    // 6. Fecha os arquivos
    fclose(bin_file);
    fclose(csv_file);

    printf("Successfully converted %zu records.\n", records_read);
    printf("Output written to: %s\n", csv_filename);

    return EXIT_SUCCESS;
}