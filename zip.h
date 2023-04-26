#include "structshuffman.h"

struct tree
{
    void *byte;
    long frequency;
    TREE *next, *left, *right;
};

//cria um novo no da arvore
TREE* new_tree_node(void *i, long frequency)
{
    TREE *new_node = (TREE*)malloc(sizeof(TREE));
    if(new_node == NULL)
    {
        printf("\nError allocating node from tree\n");
        exit(1);
    }
    new_node->byte = (void*)malloc(1);
    if(new_node->byte == NULL)
    {
        printf("\nError allocating byte\n");
        exit(1);
    }
    memcpy(new_node->byte, i, 1);
    new_node->frequency = frequency;
    new_node->left = NULL;
    new_node->next = NULL;
    new_node->right = NULL;

    return new_node;
}

//insere um novo elemento na lista de prioridade
void enqueue(TREE **tree_node, TREE *node)
{
    TREE *new_queue_node = node;

    if((*tree_node) == NULL || (*tree_node)->frequency >= new_queue_node->frequency)
    {
        new_queue_node->next = *tree_node;
        *tree_node = new_queue_node;
    }
    else
    {
        TREE *current = *tree_node;
        while(current->next != NULL)
        {
            if(current->next->frequency >= new_queue_node->frequency)
            {
                break;
            }
            current = current->next;
        }   
        new_queue_node->next = current->next;
        current->next = new_queue_node;
    }
    return;
}

//retira um elemento da lista de priorirade
TREE* dequeue(TREE **queue)
{
    if(queue == NULL)
    {
        return NULL;
    }
    TREE *dequeued = *queue;
    *queue = dequeued->next;
    dequeued->next = NULL;
    return dequeued;
}

//cria um novo no na arvore de huffman
TREE* new_huffman_node(TREE *left, TREE *right, void *byte)
{
    TREE *root = (TREE*)malloc(sizeof(TREE));
    if(root == NULL)
    {
        printf("\nError allocating root\n");
        exit(1);
    }
    root->byte = (void*)malloc(1);
    if(root->byte == NULL)
    {
        printf("\nError allocating byte to root\n");
        exit(1);
    }
    memcpy(root->byte, byte, 1);
    root->frequency = (left->frequency) + (right->frequency);
    root->left = left;
    root->right = right;

    return root;
}

//cria a nossa arvore de huffman
void insert_huffman_tree(TREE **queue, TREE **huffman_tree)
{
    uint8_t byte = '*';
    //montando a arvore de huffman
    while((*queue)->next != NULL)
    {
        TREE *left = dequeue(&*queue), *right = dequeue(&*queue), *root;
        root = new_huffman_node(left, right, &byte);
        enqueue(&*queue, root);
    }

    *huffman_tree = dequeue(&*queue);
    return;
}

//calcula o tamnho da arvore de huffman
long height(TREE *tree_node)
{
    int left, right;
    if(tree_node == NULL)
    {
        return -1;
    }
    else
    {
        left = height(tree_node->left)+1;
        right = height(tree_node->right)+1;
        if(left > right)
        {
            return left;
        }
        else
        {
            return right;
        }
    }
}

//calcula o tamanho da arvore de huffman
long tree_size(TREE *tree_node)
{
    if(tree_node == NULL)
    {
        return 0;
    }
    return 1 + (tree_node->left == NULL && tree_node->left == NULL && (*(unsigned char*)tree_node->byte == '*' || *(unsigned char*)tree_node->byte == '\\')) + tree_size(tree_node->left) + tree_size(tree_node->right);
}

//cria o dicionario
uint8_t** create_dicionary(int tree_size)
{
    int i;
    uint8_t **new_dicionary = (uint8_t**)malloc(sizeof(uint8_t*) * Max_table);
    if(new_dicionary == NULL)
    {
        printf("\nError allocating dictionary\n");
        exit(1);
    }
    for(i = 0; i < Max_table; i++)
    {
        new_dicionary[i] = (uint8_t*)malloc(sizeof(uint8_t) * tree_size);
        if(new_dicionary[i] == NULL)
        {
            printf("\nError allocating rest of dictionary\n");
            exit(1);
        }
        new_dicionary[i][0] = '\0';
    }

    return new_dicionary;
}

//preenche o dicionario
void generate_codes(uint8_t **dicionary, TREE *root, long depth, uint8_t *aux)
{
    if(root->left == NULL && root->right == NULL)
    {
        uint8_t byte = *(uint8_t*)root->byte;
        for(int i = 0; i < depth; i++)
        {
            dicionary[byte][i] = aux[i];
        }
        dicionary[byte][depth] = '\0';
        return;
    }

    aux[depth] = '0';
    generate_codes(dicionary, root->left, depth + 1, aux);

    aux[depth] = '1';
    generate_codes(dicionary, root->right, depth + 1, aux);
}

//pega o lixo de bits gerados
int trash(uint8_t **dicionary, long *freq){
    long bits_before = 0;
    long bits_after = 0;
    for(int i = 0; i < Max_table; i++){
        if (freq[i] != 0)
        {
            bits_before += freq[i] * 8;
            bits_after += freq[i] * strlen(dicionary[i]);
        }
    }

    printf("\nbits before: %ld bits_after: %ld\n", bits_before, bits_after);

    return (bits_before - bits_after) % 8;
}

//escreve a arvore de huffman no arquivo compactado
void write_tree_in_header(FILE *compressed_file, TREE *tree)
{
    if(tree == NULL)
    {
        return;
    }
    if(tree->left == NULL && tree->next == NULL && (*(uint8_t*)tree->byte == '*' || *(uint8_t*)tree->byte == '\\'))
    {
        uint8_t scape = (uint8_t)'\\';
        fwrite(&scape, sizeof(uint8_t), 1, compressed_file);
    }
    uint8_t byte = *(uint8_t*)tree->byte;
    fwrite(&byte, sizeof(uint8_t), 1, compressed_file);
    write_tree_in_header(compressed_file, tree->left);
    write_tree_in_header(compressed_file, tree->right);
}

//escreve o cabeçalho no arquivo compactado
void write_header_in_file(FILE *compressed_file, int bits_trash, int tree_sizee, TREE *tree)
{
    //setando os bits
    uint16_t trash_and_tree_size = 0, mask;
    //guarda o lixo no vetor para ser gravado no arquivo
    trash_and_tree_size |= bits_trash << 13;
    //gravando o tamanho da arvore no vetor
    int i;
    for(i = 0; i < 13; i++)
    {
        mask = 1 << i;
        if(tree_sizee & mask)
        {
            trash_and_tree_size |= 1 << i;
        }
        else
        {
            trash_and_tree_size |= 0 << i;
        }
    }
    //Como a ordem em que a fwrite escreve depende da arquitetura do processador, ou seja, little-endian ou big-endian precisamos garantir que
    //os dois bytes para lixo e para tamanho da arvore sejam escritos corretamente, para isso usazmos a funcao htons da biblioteca arpa/inet.h
    trash_and_tree_size = htons(trash_and_tree_size);
    //gravando o lixo e o tamanho da arvore
    fwrite(&trash_and_tree_size, 1, 2, compressed_file);
    //gravando a arvore
    write_tree_in_header(compressed_file, tree);
}

//escreve os bits compactados no arquivo
void write_compressed_bytes(FILE *compressed_file, uint8_t *data, uint8_t **dicionary, long file_size)
{
    long i, j, bit_index = 7;
    uint8_t byte = 0;
    int new_byte;
    for(i = 0; i < file_size; i++)
    {
        for(j = 0; dicionary[data[i]][j] != '\0'; j++)
        {
            if(dicionary[data[i]][j] == '1')
            {
                byte |= 1 << bit_index;
            }
            bit_index--;
            if(bit_index < 0)
            {
                fwrite(&byte, sizeof(unsigned char), 1, compressed_file);
                bit_index = 7;
                byte = 0;
            }
        }
    }
    if(bit_index != 7)
    {
        fwrite(&byte, sizeof(unsigned char), 1, compressed_file);
    }

    return;
}

//limpa a memoria alocada para o dicionario
void free_dicionary(uint8_t **dicionary)
{
    int i;
    for(i = 0; i < 256; i++)
    {
        free(dicionary[i]);
    }
    free(dicionary);
    return;
}

//limpa a memoria alocada para a arvore de huffman
void free_huffman_tree(TREE *root)
{
    if(root == NULL)
    {
        return;
    }
    free_huffman_tree(root->left);
    free_huffman_tree(root->right);
    free(root->byte);
    free(root);
    return;
}

//printa a arvore em pre-ordem
void pre_order(TREE *hft)
{
    if(hft != NULL)
    {
        printf("%c %ld\n", *(char*)hft->byte, hft->frequency);
        pre_order(hft->left);
        pre_order(hft->right);
    }
}

void compress(char *archive_name)
{
    //criando variáveis
    //cria a fila de frequências
    TREE *queue = NULL;
    //criando arvore de huffman
    TREE *hft = NULL;
    //criando a variavel do dicionario
    uint8_t **dicionary; 
    //criando a variavel que auxilia a preencher o dicionario
    uint8_t *aux;

    //abrindo o arquivo
    FILE *file = fopen(archive_name, "rb"), *compressed_file;
    if(file == NULL)
    {
        printf("\nCould not find .huff file\n");
        exit(1);
    }

    //criando a variavel que vai manter todos os bytes
    uint8_t *data;
    //criando a tabela de frequencia
    long frequency[Max_table], i;

    //lendo os bytes do arquivo
    //procura o fim do arquivo
    fseek(file, 0, SEEK_END);
    //pega o tamanho do arquivo
    long file_size = ftell(file);
    //volta o ponteiro para o inicio do arquivo
    fseek(file, 0, SEEK_SET);
    //aloca memoria para a variavel data que usaremos para pegar todos os bytes do arquivo
    data = malloc(file_size);
    if(data == NULL)
    {
        printf("\nCould not allocate memory for vector files\n");
        exit(1);
    }
    //setando os bytes para 0
    memset(data, 0, file_size);
    //pegando todos os bytes do arquivo
    fread(data, 1, file_size, file);
    //fechando o arquivo
    fclose(file);

    //iniciando as frequencias como 0
    memset(frequency, 0, Max_table*sizeof(long));

    //obtendo frequencias dos bytes
    for(i = 0; i < file_size; i++)
    {
        //pegando a frequencia de cada byte
        frequency[data[i]]++;
    }
    
    //montando a lista de frequência
    for(i = 0; i < Max_table; i++)
    {
        //avaliando se a frequencia e igual a 0 para nao pegarmos o byte que nao tem no arquivo
        if(frequency[i] != 0)
        {
            //fazendo o cast da variavel i para do tipo unsigned char
            (uint8_t)i;
            //insere na fila de frequencia de maneira organizada
            enqueue(&queue, new_tree_node(&i, frequency[i]));
        }
    }

    //criando a arvore de huffman
    insert_huffman_tree(&queue, &hft);

    //pegando a altura da arvore
    long tree_height = height(hft);
    //pegando o tamanho da arvore
    long tree_sizee = tree_size(hft);

    
    //criando o dicionário
    dicionary = create_dicionary(tree_height + 1);

    //criando o vetor auxiliar para preencher o dicionario
    aux = (uint8_t*)malloc(sizeof(uint8_t) * (tree_height + 1));
    if(aux == NULL)
    {
        printf("\nCould not allocate memory for auxiliary array\n");
        exit(1);
    }
    
    //preenchendo o dicionario
    generate_codes(dicionary, hft, 0, aux);
    
    //calculo do lixo de bits
    int bits_trash = trash(dicionary, frequency);
    //mudanca do nome, exemplo: arquivo.txt vira arquivo.txt.huff
    for(i = 0; i != 106; i++)
    {
        if(archive_name[i] == '\0' && i + 6 <= 106)
        {
            archive_name[i] = '.';
            archive_name[i + 1] = 'h';
            archive_name[i + 2] = 'u';
            archive_name[i + 3] = 'f';
            archive_name[i + 4] = 'f';
            archive_name[i + 5] = '\0';
            break;
        }
    }

    //escrevendo o arquivo comprimido
    compressed_file = fopen(archive_name,"wb");
    if(compressed_file == NULL)
    {
        printf("\nUnable to open output file\n");
        exit(1);
    }
    //escrevendo o cabecalho
    write_header_in_file(compressed_file, bits_trash, tree_sizee, hft);
    //escrevendo os bytes compactados
    write_compressed_bytes(compressed_file, data, dicionary, file_size);
    //fechando o arquivo
    fclose(compressed_file);

    printf("\nSuccessfully compressed file!!\n");

    //liberando a memoria alocada
    //libera a memoria do dicionario
    free_dicionary(dicionary);
    //libera a memoria da arvore
    free_huffman_tree(hft);
    //libera a memoria do vetor auxiliar
    free(aux);
    //libera a memoria alocada para o vetor data
    free(data);
    aux = NULL;
    dicionary = NULL;
    hft = NULL;
    data = NULL;
    
    return;
}
