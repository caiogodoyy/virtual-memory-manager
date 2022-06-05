#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define BACKSTORE_SIZE 256
#define MEMORY_SIZE 128
#define TLB_SIZE 16
#define OFFSET 8
#define MASK 255 // 00000000 11111111
#define MAX_BITS 10

struct thread_args
{
    int page_num;
    int page_off;
};

// variaveis globais
int flag, frame_num;
int thread_id, memory_id, tlb_id, tlb_hit, page_fault, pages; // contadores
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int tlb[TLB_SIZE][2];
int main_memory[MEMORY_SIZE][BACKSTORE_SIZE];

// funções
void *tlb_check(void *arg);

int main(int argc, char const *argv[])
{
    // arquivos
    FILE *addresses = fopen(argv[1], "r");
    FILE *outputs = fopen("outputs.txt", "w");
    FILE *backstore = fopen("BACKING_STORE.bin", "rb");

    // pagetable
    int *pagetable;
    pagetable = malloc(BACKSTORE_SIZE * sizeof(int));
    for (int i = 0; i < BACKSTORE_SIZE; i++)
        pagetable[i] = -1;

    // variaveis
    tlb_hit = 0, page_fault = 0, memory_id = 0, tlb_id = 0, pages = 0;
    int value, physical;

    // leitura dos endereços
    char address[MAX_BITS];
    while (fscanf(addresses, "%[^\n]", address) != EOF)
    {
        // tradução do endereço
        int logical = atoi(address);
        int page_num = (logical >> OFFSET) & MASK;
        int page_off = logical & MASK;

        flag = 0;
        // verificar na tlb
        pthread_t threads[TLB_SIZE];
        for (int i = 0; i < TLB_SIZE; i++)
        {
            thread_id = 0;
            struct thread_args arg;
            arg.page_num = page_num;
            arg.page_off = page_off;
            if (pthread_create(&(threads[i]), NULL, tlb_check, (void *)&arg))
                printf("ERRO"); // encerrar programa
        }
        for (int i = 0; i < TLB_SIZE; i++)
            pthread_join(threads[i], NULL);

        // tlb hit
        if (flag == 1)
        {
            // obter os valores para impressão
            value = main_memory[frame_num][page_off];
            physical = frame_num * BACKSTORE_SIZE + page_off;
        }
        // tlb miss
        else
        {
            // verificar na pagetable
            if (pagetable[page_num] != -1)
            {
                flag = 1;
                frame_num = pagetable[page_num];
                // atualizar a tlb
                if (strcmp(argv[3], "fifo") == 0)
                {
                    tlb[tlb_id][0] = page_num;
                    tlb[tlb_id][1] = frame_num;
                    tlb_id++;
                    if (tlb_id == TLB_SIZE)
                        tlb_id = 0;
                }
                else if (strcmp(argv[3], "lru") == 0)
                    printf("lru");
                else
                    printf("ERRO"); // encerrar programa
                // obter os valores para impressão
                value = main_memory[frame_num][page_off];
                physical = frame_num * BACKSTORE_SIZE + page_off;
            }
            // page fault
            else
            {
                page_fault++;
                // obter o frame no backstore
                signed char frame[BACKSTORE_SIZE];
                fseek(backstore, page_num * BACKSTORE_SIZE, SEEK_SET);
                fread(frame, sizeof(signed char), BACKSTORE_SIZE, backstore);

                // atualizar a memória principal
                if (strcmp(argv[2], "fifo") == 0)
                {
                    for (int i = 0; i < BACKSTORE_SIZE; i++)
                        main_memory[memory_id][i] = frame[i];
                    frame_num = memory_id;
                    memory_id++;
                    if (memory_id == MEMORY_SIZE)
                        memory_id = 0;
                }
                else if (strcmp(argv[2], "lru") == 0)
                    printf("lru");
                else
                    printf("ERRO"); // encerrar programa
                //
                for (int i = 0; i < BACKSTORE_SIZE; i++) {
                    if(pagetable[i] == frame_num)
                        pagetable[i] = -1;
                }
                for (int i = 0; i < TLB_SIZE; i++) {
                    if(tlb[i][1] == frame_num)
                        tlb[i][0] = -1;
                }
                // atualizar a pagetable
                pagetable[page_num] = frame_num;
                // atualizar a tlb
                if (strcmp(argv[3], "fifo") == 0)
                {
                    tlb[tlb_id][0] = page_num;
                    tlb[tlb_id][1] = frame_num;
                    tlb_id++;
                    if (tlb_id == TLB_SIZE)
                        tlb_id = 0;
                }
                else if (strcmp(argv[3], "lru") == 0)
                    printf("lru");
                else
                    printf("ERRO"); // encerrar programa

                // ler o endereço novamente
                tlb_hit--;
            }
        }
        if (flag == 1)
        {
            pages++;
            fprintf(outputs, "Virtual address: %d Physical address %d Value %d\n", logical, physical, value);
            fgetc(addresses);
        }
    }
    fprintf(outputs, "Number of Translated Addresses = %d\n", pages);
    fprintf(outputs, "Page Faults = %d\n", page_fault);
    fprintf(outputs, "Page Fault Rate = %.3f\n", (float)page_fault / pages);
    fprintf(outputs, "TLB Hits = %d\n", tlb_hit);
    fprintf(outputs, "TLB Hits Rate = %.3f", (float)tlb_hit / pages);

    // liberar espaço na memória
    free(pagetable);

    fclose(addresses);
    fclose(backstore);
    fclose(outputs);

    return 0;
}

void *tlb_check(void *arg)
{
    pthread_mutex_lock(&mutex);
    struct thread_args *args = (struct thread_args *)arg;
    int i = thread_id++;
    if (tlb[i][0] == args->page_num)
    {
        flag = 1;
        tlb_hit++;
        frame_num = tlb[i][1];
    }
    pthread_mutex_unlock(&mutex);
}
