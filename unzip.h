#include "structshuffman.h"

//struct para arvore de descompactacao
struct descompressed_tree
{
    void *byte;
    TREE_D *left, *right;
};

//verifica se o bit esta setado
int is_bit_i_set(uint8_t c, int i)
{
    unsigned char mask = 1 << i;
    return mask & c;
}

//pega os bits referentes aos bits lixo e ao tamanho da arvore de huffman
void trash_bits_and_tree_size_bits(int *trash, long *tree_size, uint8_t *data)
{
    int i, j, k, bit;
    unsigned char mask;
    //primeiro pegamos os bits referente ao lixo
    for(i = 7, j = 0; i >= 5; i--, j++)
    {
        bit = is_bit_i_set(data[0], i); 
        if(bit)
        {
            *trash |= 1 << j;
        }
    }
    
    //pegando os bits do tamanho da arvore
    for(i = 4, j = 12, k = 0; j >= 0; j--)
    {
        bit = is_bit_i_set(data[k], i); 
        if(bit)
        {
            *tree_size |= 1 << j;
        }
        i--;
        if(i < 0)
        {
            i = 7;
            k++;
        }
    }

    return;
}

//cria um novo no da arvore
TREE_D* new_tree_node_D(void *i)
{
    TREE_D *new_node = (TREE_D*)malloc(sizeof(TREE_D));
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
    new_node->left = NULL;
    new_node->right = NULL;

    return new_node;
}

//monta a arvore de huffman
TREE_D* unzipping_tree(TREE_D *root, uint8_t *data, int *i, long tree_size)
{
    if(*i == tree_size)
    {
        //testar outra hora com os caracteres de escape pra ver se vai dar pau
        return NULL;
    }
    else
    {
        if(data[*i] == '\\')
        {
            (*i)++;
            root = new_tree_node_D(&data[*i]);
            (*i)++;
            return root;
        }
        else if(data[*i] == '*')
        {
            root = new_tree_node_D(&data[*i]);
            (*i)++;
            root->left = unzipping_tree(root->left, data, i, tree_size);
            root->right = unzipping_tree(root->right, data, i, tree_size);
        }
        else
        {
            root = new_tree_node_D(&data[*i]);
            (*i)++;
            return root;
        }
    }

    return root;
}

//escreve no arquivo descompactado
void write_file(FILE* descompressed_file, uint8_t* data, long filesize, long i, TREE_D* tree, int trash)
{
    TREE_D* aux = tree;

    while(i < filesize)
    {
        uint8_t byte = data[i];

        for(int j = 7; j >= 0; j--)
        {
            if(i == filesize - 1 && j < trash) return;

            aux = is_bit_i_set(byte, j) ? aux->right : aux->left;

            if(!aux->right && !aux->left)
            {
                fwrite(&(*(uint8_t *)aux->byte), sizeof(uint8_t), 1, descompressed_file);
                aux = tree;
            }
        }
        i++;
    }
}

//limpa a memoria alocada para a arvore de huffman
void free_huffman_tree_D(TREE_D *root)
{
    if(root == NULL)
    {
        return;
    }
    free_huffman_tree_D(root->left);
    free_huffman_tree_D(root->right);
    free(root->byte);
    free(root);

    return;
}

//auxilia a printar a arvore na horizontal
void print_avl_node(TREE_D *tree_node, int depth) {
    if (tree_node != NULL) {
        print_avl_node(tree_node->right, depth + 1);
        for (int i = 0; i < depth; i++) {
            printf("   ");
        }
        printf("%c\n", *(char*)tree_node->byte);
        print_avl_node(tree_node->left, depth + 1);
    }
}

//printa a arvore na horizontal
void print_avl(TREE_D *tree) {
    printf("Árvore:\n");
    print_avl_node(tree, 0);
    printf("\n");
}

//printa a arvore em pre-ordem
void print_pre_order(TREE_D *bt)
{
    if (bt) 
    {
        printf("%c ", *(char*)bt->byte);
        print_pre_order(bt->left);
        print_pre_order(bt->right);
    }
}

void descompressed(char *archive_name)
{
    //abrindo o arquivo de leitura e escrita da descompactacao
    FILE *compressed_file, *descompressed_file;
    TREE_D *hft_descompressed = NULL;
    compressed_file = fopen(archive_name, "rb");
    uint8_t *data;
    int bits_trash = 0, i; 
    long tree_size = 0;
    if(compressed_file == NULL)
    {
        printf("\nCould not find the file\n");
        exit(1);
    }
    //buscando o final do arquivo
    fseek(compressed_file, 0, SEEK_END);
    //pegando o tamanho do arquivo
    long file_size = ftell(compressed_file);
    //voltando para o inicio do arquivo
    fseek(compressed_file, 0, SEEK_SET);
    //alocando memoria para a variavel data
    data = malloc(file_size);
    if(data == NULL)
    {
        printf("\nCould not allocate memory for vector files\n");
        exit(1);
    }
    //setando 0 em todas as posicos de data
    memset(data, 0, file_size);
    //pegando todos os bytes do arquivo
    fread(data, 1, file_size, compressed_file);
    //fechando o arquivo
    fclose(compressed_file);

    //pegando a quantidade de bits de lixo que temos e o tamanho da arvore
    trash_bits_and_tree_size_bits(&bits_trash, &tree_size, data);
    //montando a arvore de huffman
    i = 2;
    hft_descompressed = unzipping_tree(hft_descompressed, data, &i, tree_size + 2);

    //escrevendo arquivo descompactado
    int archive_name_size = strlen(archive_name);
    archive_name[archive_name_size - 5] = '\0';
    descompressed_file = fopen(archive_name, "wb");
    if(descompressed_file == NULL)
    {
        printf("Could not create output file\n");
        exit(1);
    }
    write_file(descompressed_file, data, file_size, i, hft_descompressed, bits_trash);

    //liberando o espaço
    free(data);
    free_huffman_tree_D(hft_descompressed);
    hft_descompressed = NULL;
    data = NULL;
}