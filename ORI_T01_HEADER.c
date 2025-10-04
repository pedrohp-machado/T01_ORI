/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Organização de Recuperação da Informação
 * Prof. Tiago A. Almeida
 *
 * Trabalho 01 - Indexação
 *
 * ========================================================================== *
 *   <<< IMPLEMENTE AQUI!!! COPIE E COLE O CONTEÚDO DESTE ARQUIVO NO AVA >>>
 * ========================================================================== */

// Bibliotecas
#include "ORI_T01_HEADER.h"
#include "utils.h"


// Defina aqui protótipos de funções auxiliares e macros (se quiser)

// Arquivos de dados
char ARQUIVO_TREINADORES[TAM_ARQUIVO_TREINADORES];
char ARQUIVO_BOLSOMONS[TAM_ARQUIVO_BOLSOMONS];
char ARQUIVO_BATALHAS[TAM_ARQUIVO_BATALHAS];
char ARQUIVO_RESULTADOS[TAM_ARQUIVO_RESULTADOS];
char ARQUIVO_TREINADOR_POSSUI_BOLSOMON[TAM_ARQUIVO_TREINADOR_POSSUI_BOLSOMON];


// Índices
treinadores_index *treinadores_idx = NULL;
bolsomons_index *bolsomons_idx = NULL;
batalhas_index *batalhas_idx = NULL;
resultados_index* resultados_idx = NULL;
treinador_possui_bolsomon_index *treinador_possui_bolsomon_idx = NULL;
preco_bolsomon_index *preco_bolsomon_idx = NULL;
data_index *data_idx = NULL;
inverted_list treinador_bolsomons_idx = {
        .treinador_bolsomons_secundario_idx = NULL,
        .treinador_bolsomons_primario_idx = NULL,
        .qtd_registros_secundario = 0,
        .qtd_registros_primario = 0,
        .tam_chave_secundaria = TAM_CHAVE_TREINADOR_BOLSOMON_SECUNDARIO_IDX,
        .tam_chave_primaria = TAM_CHAVE_TREINADOR_BOLSOMON_PRIMARIO_IDX,
};


// Contadores
unsigned qtd_registros_treinadores = 0;
unsigned qtd_registros_bolsomons = 0;
unsigned qtd_registros_batalhas = 0;
unsigned qtd_registros_resultados = 0;
unsigned qtd_registros_treinador_possui_bolsomon = 0;

// ---------------- Funções de comparação ----------------

/* Função de comparação entre chaves do índice treinadores_idx */
int qsort_treinadores_idx(const void *a, const void *b) {
	return strcmp(((treinadores_index*)a)->id_treinador, ((treinadores_index*)b)->id_treinador);
}

/* Função de comparação entre chaves do índice bolsomons_idx */
int qsort_bolsomons_idx(const void *a, const void *b) {
	// Comparando os dois IDs de bolsomon e retornando o resultado da comparação
	return strcmp(((bolsomons_index*)a)->id_bolsomon, ((bolsomons_index*)b)->id_bolsomon);
}

/* Função de comparação entre chaves do índice batalhas_idx */
int qsort_batalhas_idx(const void *a, const void *b) {
	// Comparando os dois IDs das batalhas e retornando o resultado da comparação
	return strcmp(((batalhas_index*)a)->id_batalha, ((batalhas_index*)b)->id_batalha);
}

/* Função de comparação entre chaves do índice resultados_idx */
int qsort_resultados_idx(const void *a, const void *b) {

	// Comparando os dois IDs dos treinadores e, se forem iguais, comparando os IDs das batalhas
	int cmp = strcmp(((resultados_index*)a)->id_treinador, ((resultados_index*)b)->id_treinador);
	
	if(cmp == 0)
		return strcmp(((resultados_index*)a)->id_batalha, ((resultados_index*)b)->id_batalha);
	else
		return cmp;
}

/* Função de comparação entre chaves do índice treinador_possui_bolsomon_idx */
int qsort_treinador_possui_bolsomon_idx(const void *a, const void *b) {

	// Comparando os dois IDs dos treinadores e, se forem iguais, comparando os IDs dos bolsomons
	int cmp = strcmp(((treinador_possui_bolsomon_index*)a)->id_treinador, ((treinador_possui_bolsomon_index*)b)->id_treinador);
	
	if(cmp == 0)
		return strcmp(((treinador_possui_bolsomon_index*)a)->id_bolsomon, ((treinador_possui_bolsomon_index*)b)->id_bolsomon);
	
	return cmp;
}

/* Função de comparação entre chaves do índice preco_bolsomon_idx */
int qsort_preco_bolsomon_idx(const void *a, const void *b){
	
	// Comparando os preços dos bolsomons
	double preco_a = ((preco_bolsomon_index*)a)->preco_bolsobolas;
	double preco_b = ((preco_bolsomon_index*)b)->preco_bolsobolas;

	// Retorna 1 se preco_a > preco_b, -1 se preco_a < preco_b, 0 se iguais
	int cmp = (preco_a > preco_b) - (preco_a < preco_b); 

	// Se os preços forem iguais, compara pelos IDs dos bolsomons
	if(cmp == 0)
		return strcmp(((preco_bolsomon_index*)a)->id_bolsomon, ((preco_bolsomon_index*)b)->id_bolsomon);
	
	return cmp;
}

/* Função usada na comparação entre as chaves do índice data_idx */
int qsort_data_idx(const void *a, const void *b) {

	int cmp = strcmp(((data_index*)a)->inicio, ((data_index*)b)->inicio);

	if(cmp == 0)
		return strcmp(((data_index*)a)->id_batalha, ((data_index*)b)->id_batalha);

	return cmp;
}

/* Função de comparação para a struct Info_Treinador, usada para determinar o campeão. */
int qsort_info_treinador(const void *a, const void *b) {
	Info_Treinador *a_info = (Info_Treinador*)a;
	Info_Treinador *b_info = (Info_Treinador*)b;

	// Ordenação com base nas estatísticas dos treinadores
	if(a_info->vezes_mais_tempo != b_info->vezes_mais_tempo) return b_info->vezes_mais_tempo - a_info->vezes_mais_tempo;
	if(a_info->vezes_mais_derrotados != b_info->vezes_mais_derrotados) return b_info->vezes_mais_derrotados - a_info->vezes_mais_derrotados;
	if(a_info->vezes_mais_dano != b_info->vezes_mais_dano) return b_info->vezes_mais_dano - a_info->vezes_mais_dano;
	
	return strcmp(a_info->id_treinador, b_info->id_treinador);
}

/* Função de comparação entre chaves do índice secundário de treinador_bolsomons_secundario_idx */
int qsort_treinador_bolsomons_secundario_idx(const void *a, const void *b) {
	return strcmp(((treinador_bolsomons_secundario_index*)a)->chave_secundaria, ((treinador_bolsomons_secundario_index*)b)->chave_secundaria);
}

/* Função de comparação entre chaves do índice primário da lista invertida */
int qsort_inverted_list_primary_search(const void *a, const void *b) {
	return strcmp((const char *)a, (const char *)b);
}

/* Função auxiliar para fazer a busca binária no placar dos treinadores com base o id */
int qsort_info_treinador_id(const void *a, const void *b){
	Info_Treinador *_a = (Info_Treinador *)a;
	Info_Treinador *_b = (Info_Treinador *)b; 

	return strcmp(_a->id_treinador, _b->id_treinador);
}

// ---------------- Criação do índice respectivo ----------------

void criar_treinadores_idx() {
	if(!treinadores_idx)
		treinadores_idx = malloc(MAX_REGISTROS * sizeof(treinadores_index));

	if(!treinadores_idx) {
		printf(ERRO_MEMORIA_INSUFICIENTE);
		exit(1);
	}

	for(unsigned i = 0; i < qtd_registros_treinadores; i++) {
		Treinador t = recuperar_registro_treinador(i);

		treinadores_idx[i].rrn = (strncmp(t.id_treinador, "*|", 2)) ? i : -1;

		strcpy(treinadores_idx[i].id_treinador, t.id_treinador);
	}

	qsort(treinadores_idx, qtd_registros_treinadores, sizeof(treinadores_index), qsort_treinadores_idx);
	printf(INDICE_CRIADO, "treinadores_idx");
}

void criar_bolsomons_idx() {

	// Criando o índice de bolsomons, se ele já não existir
	if(!bolsomons_idx) bolsomons_idx = malloc(MAX_REGISTROS * sizeof(bolsomons_index));

	// Verificando se a alocação foi bem sucedida
	if(!bolsomons_idx){
		printf(ERRO_MEMORIA_INSUFICIENTE);
		exit(1);
	}

	for(unsigned i = 0; i < qtd_registros_bolsomons; i++){
		// Recuperando o registro do bolsomon
		Bolsomon b = recuperar_registro_bolsomon(i);

		// Como não há remoção de bolsomons, o RRN é sempre o índice atual
		bolsomons_idx[i].rrn = i;

		strcpy(bolsomons_idx[i].id_bolsomon, b.id_bolsomon);
	}

	// Ordenando o índice de bolsomons pelo ID do bolsomon
	qsort(bolsomons_idx, qtd_registros_bolsomons, sizeof(bolsomons_index), qsort_bolsomons_idx);
	
	printf(INDICE_CRIADO, "bolsomons_idx");
}

void criar_batalhas_idx() {
	
	if(!batalhas_idx) batalhas_idx = malloc(MAX_REGISTROS * sizeof(batalhas_index));

	if(!batalhas_idx) {
		printf(ERRO_MEMORIA_INSUFICIENTE);
		exit(1);
	}

	for(unsigned i = 0; i < qtd_registros_batalhas; i++) {
		Batalha b = recuperar_registro_batalha(i);

		batalhas_idx[i].rrn = i;

		strcpy(batalhas_idx[i].id_batalha, b.id_batalha);
	}

	qsort(batalhas_idx, qtd_registros_batalhas, sizeof(batalhas_index), qsort_batalhas_idx);
	printf(INDICE_CRIADO, "batalhas_idx");
}

void criar_resultados_idx(){

	if(!resultados_idx) resultados_idx = malloc(MAX_REGISTROS * sizeof(resultados_index));

	if(!resultados_idx) {
		printf(ERRO_MEMORIA_INSUFICIENTE);
		exit(1);
	}

	for(unsigned i = 0; i < qtd_registros_batalhas; i++) {
		Resultado r = recuperar_registro_resultado(i);

		resultados_idx[i].rrn = i;

		strcpy(resultados_idx[i].id_batalha, r.id_batalha);
		strcpy(resultados_idx[i].id_treinador, r.id_batalha);
	}

	qsort(resultados_idx, qtd_registros_resultados, sizeof(resultados_index), qsort_resultados_idx);
	printf(INDICE_CRIADO, "resultados_idx");
}

void criar_treinador_possui_bolsomon_idx(){

	if(!treinador_possui_bolsomon_idx) treinador_possui_bolsomon_idx = malloc(MAX_REGISTROS * sizeof(treinador_possui_bolsomon_index));

	if(!treinador_possui_bolsomon_idx) {
		printf(ERRO_MEMORIA_INSUFICIENTE);
		exit(1);
	}

	for(unsigned i = 0; i < qtd_registros_treinador_possui_bolsomon; i++) {
	
		TreinadorPossuiBolsomon tpb = recuperar_registro_treinador_possui_bolsomon(i);

		treinador_possui_bolsomon_idx[i].rrn = i;
		strcpy(treinador_possui_bolsomon_idx[i].id_treinador, tpb.id_treinador);
		strcpy(treinador_possui_bolsomon_idx[i].id_bolsomon, tpb.id_bolsomon);
	}

	qsort(treinador_possui_bolsomon_idx, qtd_registros_treinador_possui_bolsomon, sizeof(treinador_possui_bolsomon_index), qsort_treinador_possui_bolsomon_idx);
	printf(INDICE_CRIADO, "treinador_possui_bolsomon_idx");
}

void criar_preco_bolsomon_idx() {
    if(!preco_bolsomon_idx) preco_bolsomon_idx = malloc(MAX_REGISTROS * sizeof(preco_bolsomon_index));

    if(!preco_bolsomon_idx) {
        printf(ERRO_MEMORIA_INSUFICIENTE);
        exit(1);
    }

    for(unsigned i = 0; i < qtd_registros_bolsomons; i++) {
        Bolsomon b = recuperar_registro_bolsomon(i);

        strcpy(preco_bolsomon_idx[i].id_bolsomon, b.id_bolsomon);
        preco_bolsomon_idx[i].preco_bolsobolas = b.preco_bolsobolas;
    }

    qsort(preco_bolsomon_idx, qtd_registros_bolsomons, 
        sizeof(preco_bolsomon_index), qsort_preco_bolsomon_idx);
        
    printf(INDICE_CRIADO, "preco_bolsomon_idx");
}

void criar_data_idx() {

	if(!data_idx) data_idx = malloc(MAX_REGISTROS * sizeof(data_index));

	if(!data_idx) {
		printf(ERRO_MEMORIA_INSUFICIENTE);
		exit(1);
	}

	for(unsigned i = 0; i < qtd_registros_batalhas; i++) {
		Batalha b = recuperar_registro_batalha(i);

		strcpy(data_idx[i].id_batalha, b.id_batalha);
		strcpy(data_idx[i].inicio, b.inicio);
	}

	qsort(data_idx, qtd_registros_batalhas, sizeof(data_index), qsort_data_idx);
	printf(INDICE_CRIADO, "data_idx");
}

void criar_treinador_bolsomons_idx() {

    // Inicializando os índices primário e secundário
    if (!treinador_bolsomons_idx.treinador_bolsomons_secundario_idx)
        treinador_bolsomons_idx.treinador_bolsomons_secundario_idx = malloc(MAX_REGISTROS * sizeof(treinador_bolsomons_secundario_index));

    if (!treinador_bolsomons_idx.treinador_bolsomons_primario_idx)
        treinador_bolsomons_idx.treinador_bolsomons_primario_idx = malloc(MAX_REGISTROS * sizeof(treinador_bolsomons_primario_index));

    if (!treinador_bolsomons_idx.treinador_bolsomons_secundario_idx || !treinador_bolsomons_idx.treinador_bolsomons_primario_idx) {
        printf(ERRO_MEMORIA_INSUFICIENTE);
        exit(1);
    }

	// Inicializando contadores do índice 
	treinador_bolsomons_idx.qtd_registros_primario = 0;
    treinador_bolsomons_idx.qtd_registros_secundario = 0;


    for(unsigned i = 0; i < qtd_registros_treinador_possui_bolsomon; i++){
        // Dê um nome diferente para a variável local
        TreinadorPossuiBolsomon tpb = recuperar_registro_treinador_possui_bolsomon(i);

        // Agora o resto do código funciona, pois não há ambiguidade
        bolsomons_index chave_busca;
        strcpy(chave_busca.id_bolsomon, tpb.id_bolsomon);
        
		int bolsomon_encontrado = busca_binaria(&chave_busca, bolsomons_idx, qtd_registros_bolsomons,
									sizeof(bolsomons_index), qsort_bolsomons_idx,false, 0);

        if(bolsomon_encontrado != -1){
            Bolsomon b = recuperar_registro_bolsomon(bolsomons_idx[bolsomon_encontrado].rrn);
            inverted_list_insert(b.nome, tpb.id_treinador, &treinador_bolsomons_idx);
        }
    }

	// Por fim, ordeno o índice secundário
	qsort(treinador_bolsomons_idx.treinador_bolsomons_secundario_idx, treinador_bolsomons_idx.qtd_registros_secundario, 
          sizeof(treinador_bolsomons_secundario_index), 
          qsort_treinador_bolsomons_secundario_idx);

	printf(INDICE_CRIADO, "treinador_bolsomons_idx");
}

// ---------------- Recuperação do registro ----------------

Treinador recuperar_registro_treinador(int rrn) {
	Treinador t;
	char buffer[TAM_REGISTRO_TREINADOR + 1], *p;

	strncpy(buffer, ARQUIVO_TREINADORES + (rrn * TAM_REGISTRO_TREINADOR), TAM_REGISTRO_TREINADOR);
	buffer[TAM_REGISTRO_TREINADOR] = '\0';

	p = strtok(buffer, ";");
	strcpy(t.id_treinador, p);

	p = strtok(NULL, ";");
	strcpy(t.apelido, p);

	p = strtok(NULL, ";");
	strcpy(t.cadastro, p);

	p = strtok(NULL, ";");
	strcpy(t.premio, p);

	p = strtok(NULL, ";");
	t.bolsobolas = atof(p);

	return t;
}

Bolsomon recuperar_registro_bolsomon(int rrn) {

	// Criando duas variáveis locais temporárias para fazer a recuperação e parsing do registro
	char buffer[TAM_REGISTRO_BOLSOMON + 1], *p;
	Bolsomon b;

	// Recuperando o registro bruto do arquivo a partir do rrn e colocando na variável buffer
	strncpy(buffer, ARQUIVO_BOLSOMONS + (rrn * TAM_REGISTRO_BOLSOMON), TAM_REGISTRO_BOLSOMON);
	buffer[TAM_REGISTRO_BOLSOMON] = '\0';

	// Fazendo o Parsing do registro com o delimitador ';' e construindo a struct Bolsomon
	p = strtok(buffer, ";");
	strcpy(b.id_bolsomon, p);

	p = strtok(NULL, ";");
	strcpy(b.nome, p);

	p = strtok(NULL, ";");
	strcpy(b.habilidade, p);

	p = strtok(NULL, ";");
	b.preco_bolsobolas = atof(p);

	return b;
}

Batalha recuperar_registro_batalha(int rrn) {

	//Criando duas variáveis locais temporárias para fazer a recuperação e parsing do registro
	char buffer[TAM_REGISTRO_BATALHA + 1];
	Batalha ba;

	//Recuperando o registro bruto do arquivo a partir do rrn e colocando na variável buffer
	strncpy(buffer, ARQUIVO_BATALHAS + (rrn * TAM_REGISTRO_BATALHA), TAM_REGISTRO_BATALHA);
	buffer[TAM_REGISTRO_BATALHA] = '\0';

	/* 
	*  Fazendo o Parsing do registro e construindo a struct Batalha
	*  Como o arquivo não possui um delimitador de fim de string, usamos strncpy
	*  para copiar apenas os n primeiros caracteres do ID da batalha, e coloco o \0 ao final da string
	*/ 

	strncpy(ba.id_batalha, buffer, 8);
	ba.id_batalha[8] = '\0';

	strncpy(ba.inicio, buffer + 8, 12);
	ba.inicio[12] = '\0';

	strncpy(ba.duracao, buffer + 20, 6);
	ba.duracao[6] = '\0';

	strncpy(ba.arena, buffer + 26, 4);
	ba.arena[4] = '\0';

	return ba;
}

Resultado recuperar_registro_resultado(int rrn) {
	
	char buffer[TAM_REGISTRO_RESULTADO + 1];
	strncpy(buffer, ARQUIVO_RESULTADOS + (rrn * TAM_REGISTRO_RESULTADO), TAM_REGISTRO_RESULTADO);
	buffer[TAM_REGISTRO_RESULTADO] = '\0';

	Resultado r;

	// Fazendo o Parsing do registro e construindo a struct Resultado assim como na função recuperar_registro_batalha()
	strncpy(r.id_treinador, buffer, 11);
	r.id_treinador[11] = '\0';

	strncpy(r.id_batalha, buffer + 11, 8);
	r.id_batalha[8] = '\0';

	strncpy(r.id_bolsomon, buffer + 19, 3);
	r.id_bolsomon[3] = '\0';

	// Convertendo os caracteres '1' e '0' para booleanos
	r.foi_maior_duracao = (buffer[22] == '1');

	r.foi_mais_derrotas = (buffer[23] == '1');

	r.foi_mais_dano = (buffer[24] == '1');

	return r;
}

TreinadorPossuiBolsomon recuperar_registro_treinador_possui_bolsomon(int rrn) {
	
	TreinadorPossuiBolsomon tpb; // Apenas para compilar
	char buffer[TAM_REGISTRO_TREINADOR_POSSUI_BOLSOMON + 1];

	strncpy(buffer, ARQUIVO_TREINADOR_POSSUI_BOLSOMON + (rrn * TAM_REGISTRO_TREINADOR_POSSUI_BOLSOMON), TAM_REGISTRO_TREINADOR_POSSUI_BOLSOMON);
	buffer[TAM_REGISTRO_TREINADOR_POSSUI_BOLSOMON] = '\0';
	
	strncpy(tpb.id_treinador, buffer, 11);
	tpb.id_treinador[11] = '\0';

	strncpy(tpb.id_bolsomon, buffer + 11, 3);
	tpb.id_bolsomon[3] = '\0';
	
	return tpb;
}

// ---------------- Gravação nos arquivos de dados ----------------

void escrever_registro_treinador(Treinador t, int rrn) {
	char buffer[TAM_REGISTRO_TREINADOR + 1], double_str[100];
	buffer[0] = '\0';
	double_str[0] = '\0';

	strcat(buffer, t.id_treinador); strcat(buffer, ";");
	strcat(buffer, t.apelido); strcat(buffer, ";");
	strcat(buffer, t.cadastro); strcat(buffer, ";");
    strcat(buffer, t.premio); strcat(buffer, ";");
	sprintf(double_str, "%013.2lf", t.bolsobolas);
	strcat(buffer, double_str);	strcat(buffer, ";");
    
	strpadright(buffer, '#', TAM_REGISTRO_TREINADOR);
    
	strncpy(ARQUIVO_TREINADORES + rrn * TAM_REGISTRO_TREINADOR, buffer, TAM_REGISTRO_TREINADOR);
}

void escrever_registro_bolsomon(Bolsomon b, int rrn) {

	// Criando duas variáveis locais temporárias para construir o registro
	char buffer[TAM_REGISTRO_BOLSOMON + 1], double_preco[100];
	buffer[0] = '\0';
	double_preco[0] = '\0';

	// Atribuo o valor do registro à variável buffer, formatando conforme o especificado
	strcat(buffer, b.id_bolsomon); strcat(buffer, ";"); // Coloco um ';' como delimitador após cada campo
	strcat(buffer, b.nome); strcat(buffer, ";");
	strcat(buffer, b.habilidade); strcat(buffer, ";");
	sprintf(double_preco, "%013.2lf", b.preco_bolsobolas); // Formato o double para uma string com 2 casas decimais e 13 caracteres
	strcat(buffer, double_preco); strcat(buffer, ";");

	// Preencho o restante do registro com '#' para completar o tamanho fixo
	strpadright(buffer, '#', TAM_REGISTRO_BOLSOMON);

	// Finalmente, copio o registro formatado para o arquivo de dados na posição correta
	strncpy(ARQUIVO_BOLSOMONS + rrn * TAM_REGISTRO_BOLSOMON, buffer, TAM_REGISTRO_BOLSOMON);
}

void escrever_registro_batalha(Batalha b, int rrn) {

	// Mesma rotina de escrever_registro_bolsomon(), adaptada para a struct Batalha

	char buffer[TAM_REGISTRO_BATALHA + 1];
	buffer[0] = '\0';

	strcat(buffer, b.id_batalha); 
	strcat(buffer, b.inicio); 
	strcat(buffer, b.duracao);
	strcat(buffer, b.arena);

	strpadright(buffer, '#', TAM_REGISTRO_BATALHA);

	strncpy(ARQUIVO_BATALHAS + rrn * TAM_REGISTRO_BATALHA, buffer, TAM_REGISTRO_BATALHA);

}

void escrever_registro_resultado(Resultado r, int rrn) {

	char buffer[TAM_REGISTRO_RESULTADO + 1];
	buffer[0] = '\0';

	// Para garantir que os campos tenham o tamanho correto, uso sprintf com especificadores de largura
	// Isso é necessário pois os campos não possuem delimitadores e têm tamanhos fixos
	sprintf(buffer, "%.11s%.8s%.3s", r.id_treinador, r.id_batalha, r.id_bolsomon);

	// Convertendo os booleanos para '1' e '0' e concatenando ao buffer
	strcat(buffer, r.foi_maior_duracao ? "1" : "0"); 
	strcat(buffer, r.foi_mais_derrotas ? "1" : "0");
	strcat(buffer, r.foi_mais_dano ? "1" : "0"); 

	strncpy(ARQUIVO_RESULTADOS + rrn * TAM_REGISTRO_RESULTADO, buffer, TAM_REGISTRO_RESULTADO);
}

void escrever_registro_treinador_possui_bolsomon(TreinadorPossuiBolsomon tpb, int rrn) {

	char buffer[TAM_REGISTRO_TREINADOR_POSSUI_BOLSOMON + 1];
	buffer[0] = '\0';

	sprintf(buffer, "%.11s%.3s", tpb.id_treinador, tpb.id_bolsomon);

	strncpy(ARQUIVO_TREINADOR_POSSUI_BOLSOMON + rrn * TAM_REGISTRO_TREINADOR_POSSUI_BOLSOMON, buffer, TAM_REGISTRO_TREINADOR_POSSUI_BOLSOMON);
}

// ---------------- Exibição dos registros ----------------

bool exibir_treinador(int rrn) {
	if(rrn < 0) return false;
    
	Treinador t = recuperar_registro_treinador(rrn);
    
	printf("%s, %s, %s, %s, %.2lf\n", t.id_treinador, t.apelido, t.cadastro, t.premio, t.bolsobolas);
    
	return true;
}

bool exibir_bolsomon(int rrn) {
	if(rrn < 0 || rrn >= qtd_registros_bolsomons) return false;

	Bolsomon bo = recuperar_registro_bolsomon(rrn);
	printf("%s, %s, %s, %.2lf\n", bo.id_bolsomon, bo.nome, bo.habilidade, bo.preco_bolsobolas);
	return true;
}

bool exibir_batalha(int rrn) {
	if(rrn < 0 || rrn >= qtd_registros_batalhas) return false;

	Batalha ba = recuperar_registro_batalha(rrn);
	printf("%s, %s, %s, %s\n", ba.id_batalha, ba.inicio, ba.duracao, ba.arena);
	return true;
}

// ---------------- Funções principais ----------------

void cadastrar_treinador_menu(char *id_treinador, char *apelido) {
	
	// Primeiro, busco o rrn do treinador no índice para verificar se ele já existe
	treinadores_index chave_busca;
	strcpy(chave_busca.id_treinador, id_treinador);
	int encontrado = busca_binaria(&chave_busca, treinadores_idx, qtd_registros_treinadores, 
                                        sizeof(treinadores_index), qsort_treinadores_idx, 
                                        false, 0);
	
	if(encontrado != -1 && treinadores_idx[encontrado].rrn != -1){
        printf(ERRO_PK_REPETIDA, id_treinador);
        return;
    }

	// Se o treinador não existir, crio a struct Treinador com os dados fornecidos
	Treinador novo_treinador;
	strcpy(novo_treinador.id_treinador, id_treinador);
	strcpy(novo_treinador.apelido, apelido);
	novo_treinador.bolsobolas = 0.0;
	strcpy(novo_treinador.premio, "000000000000"); // Inicializo o campo premio como string vazia
	current_datetime(novo_treinador.cadastro); // Preencho o campo cadastro com a data atual

	// Escrevo o novo treinador no arquivo de dados na posição do próximo RRN disponível
	escrever_registro_treinador(novo_treinador, qtd_registros_treinadores);

	// Atualizo o índice de treinadores e a quantidade de registros
	treinadores_idx[qtd_registros_treinadores].rrn = qtd_registros_treinadores;
	strcpy(treinadores_idx[qtd_registros_treinadores].id_treinador, id_treinador);
	qtd_registros_treinadores++;

	// Reordeno o índice de treinadores para manter a ordenação
	qsort(treinadores_idx, qtd_registros_treinadores, sizeof(treinadores_index), qsort_treinadores_idx);

	printf(SUCESSO);
}

void remover_treinador_menu(char *id_treinador) {

	// Primeiro, busco o rrn do treinador no índice para verificar se ele existe
	treinadores_index chave_busca;
	strcpy(chave_busca.id_treinador, id_treinador);
	int encontrado = busca_binaria(&chave_busca, treinadores_idx, qtd_registros_treinadores, 
                                   sizeof(treinadores_index), qsort_treinadores_idx, false, 0);

	if(encontrado == -1 || treinadores_idx[encontrado].rrn == -1){
		// Se o treinador não existir, exibo a mensagem de erro e retorno
		printf(ERRO_REGISTRO_NAO_ENCONTRADO);
		return;
	}

	// Se o treinador existir, marco o registro como removido no arquivo de dados
	char *p = ARQUIVO_TREINADORES + (treinadores_idx[encontrado].rrn * TAM_REGISTRO_TREINADOR);

	// Faço atribuiçao manual para evitar o warning do strncpy
	p[0] = '*'; p[1] = '|';

	// Atualizo o índice de treinadores marcando o RRN como -1 (removido)
	treinadores_idx[encontrado].rrn = -1;

	printf(SUCESSO);
}

void adicionar_bolsobolas_menu(char *id_treinador, double valor) {
	adicionar_bolsobolas(id_treinador, valor, true);
}

void adicionar_bolsobolas(char *id_treinador, double valor, bool flag){

	// Verifico se o valor adicionado é positivo
	if(valor <=0){
		printf(ERRO_VALOR_INVALIDO);
		return;
	}

	// Busco o treinador no índice
	treinadores_index chave_busca;
	strcpy(chave_busca.id_treinador, id_treinador);
	int encontrado = busca_binaria(&chave_busca, treinadores_idx, qtd_registros_treinadores,
									sizeof(treinadores_index), qsort_treinadores_idx, false, 0);
	
	if(encontrado == -1 || treinadores_idx[encontrado].rrn == -1){
		// Se o treinador não existir, exibo a mensagem de erro e retorno
		printf(ERRO_REGISTRO_NAO_ENCONTRADO);
		return;
	}

	// Se o treinador existir, recupero o registro e atualizo o campo de bolsobolas
	Treinador t = recuperar_registro_treinador(treinadores_idx[encontrado].rrn);
	t.bolsobolas += valor;

	// Escrevo o registro atualizado de volta no arquivo de dados
	escrever_registro_treinador(t, treinadores_idx[encontrado].rrn );

	if(flag) printf(SUCESSO);	
}

void cadastrar_bolsomon_menu(char *nome, char *habilidade, double preco) {

	// Crio a struct do novo bolsomon com os dados fornecidos
	Bolsomon novo_bolsomon;
	strcpy(novo_bolsomon.nome, nome);
	strcpy(novo_bolsomon.habilidade, habilidade);
	novo_bolsomon.preco_bolsobolas = preco;

	// Gero um novo ID para o bolsomon baseado na quantidade atual de registros
	unsigned novo_id = qtd_registros_bolsomons;
	sprintf(novo_bolsomon.id_bolsomon, "%03u", novo_id); // Formato o ID com 3 dígitos, preenchendo com zeros à esquerda

	// Escrevo o novo registro no arquivo de dados na posição do próximo RRN disponível
	escrever_registro_bolsomon(novo_bolsomon, qtd_registros_bolsomons);

	// Atualizo o índice de bolsomons e a quantidade de registros
	bolsomons_idx[qtd_registros_bolsomons].rrn = qtd_registros_bolsomons;
	strcpy(bolsomons_idx[qtd_registros_bolsomons].id_bolsomon, novo_bolsomon.id_bolsomon);

	// Também atualizo o índice de preço de bolsomons, se ele existir
	if(preco_bolsomon_idx){
		preco_bolsomon_idx[qtd_registros_bolsomons].preco_bolsobolas = preco;
		strcpy(preco_bolsomon_idx[qtd_registros_bolsomons].id_bolsomon, novo_bolsomon.id_bolsomon);
	}

	qtd_registros_bolsomons++;

	// Reordeno os índices para manter a ordenação
	qsort(bolsomons_idx, qtd_registros_bolsomons, sizeof(bolsomons_index), qsort_bolsomons_idx);
	qsort(preco_bolsomon_idx, qtd_registros_bolsomons, sizeof(preco_bolsomon_index), qsort_preco_bolsomon_idx);

	printf(SUCESSO);
}

void comprar_bolsomon_menu(char *id_treinador, char *id_bolsomon) {

	// Busco o treinador no índice
	treinadores_index chave_treinador;
	strcpy(chave_treinador.id_treinador, id_treinador);
	int treinador_encontrado = busca_binaria(&chave_treinador, treinadores_idx, 
		qtd_registros_treinadores, sizeof(treinadores_index), qsort_treinadores_idx, false, 0);
	
	// Verifico se o treinador existe
	if(treinador_encontrado == -1 || treinadores_idx[treinador_encontrado].rrn == -1){
		printf(ERRO_REGISTRO_NAO_ENCONTRADO);
		return;
	}

	// Busco o bolsomon no índice
	bolsomons_index chave_bolsomon;
	strcpy(chave_bolsomon.id_bolsomon, id_bolsomon);
	int bolsomon_encontrado = busca_binaria(&chave_bolsomon, bolsomons_idx, 
		qtd_registros_bolsomons, sizeof(bolsomons_index), qsort_bolsomons_idx, false, 0);

	// Verifico se o bolsomon existe
	if(bolsomon_encontrado == -1 || bolsomons_idx[bolsomon_encontrado].rrn == -1){
		printf(ERRO_REGISTRO_NAO_ENCONTRADO);
		return;
	}

	// Verifico se a relação treinador - bolsomon já existe
	treinador_possui_bolsomon_index chave_tpb;
	strcpy(chave_tpb.id_treinador, id_treinador);
	strcpy(chave_tpb.id_bolsomon, id_bolsomon);
	int tpb_encontrado = busca_binaria(&chave_tpb, treinador_possui_bolsomon_idx, 
		qtd_registros_treinador_possui_bolsomon, sizeof(treinador_possui_bolsomon_index), 
		qsort_treinador_possui_bolsomon_idx, false, 0);
	
	// Se a relação já existir, exibo a mensagem de erro e retorno
	if(tpb_encontrado != -1 && treinador_possui_bolsomon_idx[tpb_encontrado].rrn != -1){
		printf(ERRO_BOLSOMON_REPETIDO, id_treinador, id_bolsomon);
		return;
	}

	unsigned treinador_rrn = treinadores_idx[treinador_encontrado].rrn;
	unsigned bolsomon_rrn = bolsomons_idx[bolsomon_encontrado].rrn; 

	// Se ambos existirem, verifico se o treinador tem bolsobolas para comprar o bolsomon
	Treinador t = recuperar_registro_treinador(treinador_rrn);
	Bolsomon b = recuperar_registro_bolsomon(bolsomon_rrn);

	if(t.bolsobolas < b.preco_bolsobolas){
		printf(ERRO_SALDO_NAO_SUFICIENTE);
		return;
	}

	// Se o treinador tem saldo suficiente, desconto o preço do bolsomon
	t.bolsobolas -= b.preco_bolsobolas;

	// Então, escrevo a nova relação treinador - bolsomon no arquivo de dados TreinadorpossuiBolsomon
	TreinadorPossuiBolsomon tpb;
	strcpy(tpb.id_treinador, id_treinador);
	strcpy(tpb.id_bolsomon, id_bolsomon);

	// Escrevo o novo registro no índice na última posição disponível
	treinador_possui_bolsomon_idx[qtd_registros_treinador_possui_bolsomon].rrn = qtd_registros_treinador_possui_bolsomon;
	strcpy(treinador_possui_bolsomon_idx[qtd_registros_treinador_possui_bolsomon].id_treinador, id_treinador);
	strcpy(treinador_possui_bolsomon_idx[qtd_registros_treinador_possui_bolsomon].id_bolsomon, id_bolsomon);

	// Escrevo o registro no arquivo de dados 
	escrever_registro_treinador_possui_bolsomon(tpb, qtd_registros_treinador_possui_bolsomon);

	// Reescrevo o registro atualizado do treinador no arquivo de dados
	escrever_registro_treinador(t, treinador_rrn);

	// Atualizo o índice da lista invertida de treinador - bolsomons e o índice de treinador_possui_bolsomon
	inverted_list_insert(b.nome, id_treinador, &treinador_bolsomons_idx);
	strcpy(treinador_possui_bolsomon_idx[qtd_registros_treinador_possui_bolsomon].id_bolsomon, id_bolsomon);
	strcpy(treinador_possui_bolsomon_idx[qtd_registros_treinador_possui_bolsomon].id_treinador, id_treinador);
	treinador_possui_bolsomon_idx[qtd_registros_treinador_possui_bolsomon].rrn = qtd_registros_treinador_possui_bolsomon;

	// Atualizo a quantidade de registros
	qtd_registros_treinador_possui_bolsomon++;

	// Faço a reordenação do índice de treinador_possui_bolsomon
	qsort(treinador_possui_bolsomon_idx, qtd_registros_treinador_possui_bolsomon, 
		sizeof(treinador_possui_bolsomon_index), qsort_treinador_possui_bolsomon_idx);

	// Faço a reordenação do índice secundário de treinador_bolsomons
	qsort(treinador_bolsomons_idx.treinador_bolsomons_secundario_idx, treinador_bolsomons_idx.qtd_registros_secundario, 
		  sizeof(treinador_bolsomons_secundario_index), 
		  qsort_treinador_bolsomons_secundario_idx);

	printf(SUCESSO);
}

void executar_batalha_menu(char *inicio, char *duracao, char *arena, char *id_treinadores, char *bolsomons_treinadores, char *duracoes_treinadores, char *eliminacoes_treinadores, char *danos_causados_treinadores) {

	// Recebo os dados dos parâmetros e os separo em arrays para facilitar o manuseio
	char treinadores[QTD_MAX_TREINADORES][TAM_ID_TREINADOR];
	// Como cada treinador entra com um bolsomon, crio uma matriz para armazenar os IDs dos bolsomons
	char bolsomons[QTD_MAX_TREINADORES][TAM_ID_BOLSOMON];

	// Preencho os campos com base no tamanho fixo dos IDs
	for(unsigned i = 0; i < QTD_MAX_TREINADORES; i++){
		strncpy(treinadores[i], id_treinadores + i * (TAM_ID_TREINADOR - 1), TAM_ID_TREINADOR - 1);
		treinadores[i][TAM_ID_TREINADOR - 1] = '\0'; // Garantindo que a string termine com \0
		strncpy(bolsomons[i], bolsomons_treinadores + i * (TAM_ID_BOLSOMON - 1), TAM_ID_BOLSOMON - 1);
		bolsomons[i][TAM_ID_BOLSOMON - 1] = '\0';
	}

	// Verifico se os registros são válidos
	for(unsigned i = 0; i < QTD_MAX_TREINADORES; i++){
		// Verifico se o treinador existe
		treinadores_index chave_treinador;
		strcpy(chave_treinador.id_treinador, treinadores[i]);
		int treinador_encontrado = busca_binaria(&chave_treinador, treinadores_idx, qtd_registros_treinadores,
												sizeof(treinadores_index), qsort_treinadores_idx, false, 0);
		
		if(treinador_encontrado == -1 || treinadores_idx[treinador_encontrado].rrn == -1){
			printf(ERRO_REGISTRO_NAO_ENCONTRADO);
			return;
		}

		// Verifico se o bolsomon existe
		bolsomons_index chave_bolsomon;
		strcpy(chave_bolsomon.id_bolsomon, bolsomons[i]);
		int bolsomon_encontrado = busca_binaria(&chave_bolsomon, bolsomons_idx, qtd_registros_bolsomons,
												sizeof(bolsomons_index), qsort_bolsomons_idx, false, 0);
		if(bolsomon_encontrado == -1){
			printf(ERRO_REGISTRO_NAO_ENCONTRADO);
			return;
		}

		// Verifico se o treinador possui o bolsomon
		treinador_possui_bolsomon_index chave_tpb;
		strcpy(chave_tpb.id_treinador, treinadores[i]);
		strcpy(chave_tpb.id_bolsomon, bolsomons[i]);
		int tpb_encontrado = busca_binaria(&chave_tpb, treinador_possui_bolsomon_idx, qtd_registros_treinador_possui_bolsomon,
											sizeof(treinador_possui_bolsomon_index), qsort_treinador_possui_bolsomon_idx,
											false, 0);
		if(tpb_encontrado == -1){
			Bolsomon b = recuperar_registro_bolsomon(bolsomons_idx[bolsomon_encontrado].rrn);
			printf(ERRO_TREINADOR_BOLSOMON, treinadores[i], b.nome);
			return;
		}
	}

	// Crio a struct de batalha com os parametros fornecidos
	Batalha ba;
	strcpy(ba.inicio, inicio);
	strcpy(ba.duracao, duracao);
	strcpy(ba.arena, arena);
	sprintf(ba.id_batalha, "%08u", qtd_registros_batalhas);

	// Atualizo o índice de batalhas
	strcpy(batalhas_idx[qtd_registros_batalhas].id_batalha, ba.id_batalha);
	batalhas_idx[qtd_registros_batalhas].rrn = qtd_registros_batalhas;

	// Atualizo o índice de data das batalhas
	strcpy(data_idx[qtd_registros_batalhas].id_batalha, ba.id_batalha);
	strcpy(data_idx[qtd_registros_batalhas].inicio, ba.inicio);

	// Escrevo a batalha no arquivo
	escrever_registro_batalha(ba, qtd_registros_batalhas);

	// Atualizo a quantidade de batalhas
	qtd_registros_batalhas++;

	// Reordeno os índices de batalhas e índices
	qsort(batalhas_idx, qtd_registros_batalhas, sizeof(batalhas_index),
		qsort_batalhas_idx);
	qsort(data_idx, qtd_registros_batalhas, sizeof(data_index), qsort_data_idx);

	// Por fim, crio o registro da batalha analisando as estatisticas da batalha
	
	// Pego os dados da batalha e atribuo 
	int duracoes[QTD_MAX_TREINADORES], eliminacoes[QTD_MAX_TREINADORES];
	long int danos[QTD_MAX_TREINADORES];

	// Crio variáveis auxiliares para ajudar na análise
	int maior_duracao = -1, mais_eliminacoes = -1;
	long int maior_dano = -1;

	// Recebo as estatisticas das batalha
	for(unsigned i = 0; i < QTD_MAX_TREINADORES; i++){
		// Variáeis auxiliares para receber os dados 
		char duracao_batalha[TAM_TIME], eliminacao_batalha[TAM_INT_NUMBER], dano_batalha[TAM_INT_NUMBER];
		strncpy(duracao_batalha, duracoes_treinadores + i * (TAM_TIME - 1), TAM_TIME - 1); duracao_batalha[TAM_TIME - 1] = '\0';
		strncpy(eliminacao_batalha, eliminacoes_treinadores + i * (TAM_INT_NUMBER - 1), TAM_INT_NUMBER - 1); eliminacao_batalha[TAM_INT_NUMBER - 1] = '\0';
		strncpy(dano_batalha, danos_causados_treinadores + i * (TAM_INT_NUMBER - 1), TAM_INT_NUMBER - 1); dano_batalha[TAM_INT_NUMBER - 1] = '\0';

		// Converto os valores
		duracoes[i] = atoi(duracao_batalha);
		eliminacoes[i] = atoi(eliminacao_batalha);
		danos[i] = atol(dano_batalha);

		// Atualizo as estatisticas
		if(duracoes[i] > maior_duracao) maior_duracao = duracoes[i];
		if(eliminacoes[i] > mais_eliminacoes) mais_eliminacoes = eliminacoes[i];
		if(danos[i] > maior_dano) maior_dano = danos[i];
	}	
	
	// Faço a inserção dos resultados
	for(unsigned i = 0; i < QTD_MAX_TREINADORES; i++){
		Resultado r;
		strcpy(r.id_treinador, treinadores[i]);
		strcpy(r.id_bolsomon, bolsomons[i]);
		strcpy(r.id_batalha, ba.id_batalha);
		
		// Verifico se foi o 'melhor' resultado pelo índice do treinador no array de treinadores
		r.foi_maior_duracao = (duracoes[i] == maior_duracao);
		r.foi_mais_dano = (danos[i] == maior_dano);
		r.foi_mais_derrotas = (eliminacoes[i] == mais_eliminacoes);

		// Atualizo o índice de resultados
		strcpy(resultados_idx[qtd_registros_resultados].id_batalha, ba.id_batalha);
		strcpy(resultados_idx[qtd_registros_resultados].id_treinador, r.id_treinador);
		resultados_idx[qtd_registros_resultados].rrn = qtd_registros_resultados;

		// Atualizo o arquivo de resultados 
		escrever_registro_resultado(r, qtd_registros_resultados);

		// Atualizo o contador de registros de resultados
		qtd_registros_resultados++;
	}

	// Ordeno o índice de resultados
	qsort(resultados_idx, qtd_registros_resultados, sizeof(resultados_index), qsort_resultados_idx);
	printf(SUCESSO);
}

void recompensar_campeao_menu(char *data_inicio, char *data_fim, double premio) {
	
	// Struct para armazenar as estatísticas dos treinadores dentro do período
	Info_Treinador placares[MAX_REGISTROS];
	char batalhas_periodo[MAX_REGISTROS][TAM_CHAVE_BATALHAS_IDX];
	int qtd_participantes = 0, qtd_batalhas = 0;

	// Procuro o rrn da primeira batalha do período
	data_index chave_data_batalha;
	strcpy(chave_data_batalha.inicio, data_inicio);
	int primeira_batalha = busca_binaria(&chave_data_batalha, data_idx, qtd_registros_batalhas, sizeof(data_index), 
										qsort_data_idx, false, 1);

	// Percorro o índice de batalhas para pegar as batalhas válidas
	for(unsigned i = primeira_batalha; i < qtd_registros_batalhas; i++){
		// Verifico se a batalha está no intervalo 
		if(strcmp(data_idx[i].inicio, data_fim) > 0) break;
		// Add a batalha no array
		strcpy(batalhas_periodo[qtd_batalhas], data_idx[i].id_batalha);
		qtd_batalhas++;
	}

	// Verifico se há uma batalha no período
	if(!qtd_batalhas){
		printf(AVISO_NENHUM_REGISTRO_ENCONTRADO);
		return;
	}

	qsort(batalhas_periodo, qtd_batalhas, sizeof(batalhas_periodo[0]), qsort_inverted_list_primary_search);

	// Inicializo o array com os placares 
	for(unsigned i = 0; i < qtd_registros_treinadores; i++){
		strcpy(placares[qtd_participantes].id_treinador, treinadores_idx[i].id_treinador);
		placares[qtd_participantes].vezes_mais_dano = 0; 
		placares[qtd_participantes].vezes_mais_tempo = 0; 
		placares[qtd_participantes].vezes_mais_derrotados = 0;
		qtd_participantes++;
	}

	// Com o id das batalhas, recupero seus resultados
	for (unsigned i = 0; i < qtd_registros_resultados; i++) {
        // Pega o id_batalha do resultado atual
        char *id_batalha_resultado = resultados_idx[i].id_batalha;

        // Verifica (com busca binária) se essa batalha está no nosso período
        batalhas_index chave_batalha;
        strcpy(chave_batalha.id_batalha, id_batalha_resultado);
        if (busca_binaria(&chave_batalha, batalhas_periodo, qtd_batalhas, sizeof(batalhas_periodo[0]), qsort_inverted_list_primary_search, false, 0) != -1) {
            
            // Se a batalha é relevante, processa o resultado
            Resultado r = recuperar_registro_resultado(resultados_idx[i].rrn);

            // Encontro o treinador no nosso array de placares e atualiza os pontos
			Info_Treinador chave_info;
			strcpy(chave_info.id_treinador, r.id_treinador);
			int treinador_encontrado = busca_binaria(&chave_info, placares, qtd_participantes, sizeof(Info_Treinador),
													qsort_info_treinador_id, false, 0);
			// Contabilizo as estatísticas
			if(treinador_encontrado != -1){
				if(r.foi_mais_dano) placares[treinador_encontrado].vezes_mais_dano++;
				if(r.foi_mais_derrotas) placares[treinador_encontrado].vezes_mais_derrotados++;
				if(r.foi_maior_duracao) placares[treinador_encontrado].vezes_mais_tempo++;
			}
        }
    }

	// Ordeno os placares com base nas estatísticas 
	qsort(placares, qtd_participantes, sizeof(Info_Treinador), qsort_info_treinador);

	// Percorro os placares e recompenso o primeiro treinador que não foi removido
	for(unsigned i = 0; i < qtd_participantes; i++){
		// Verifico se o treinador existe
		treinadores_index chave_treinador;
		strcpy(chave_treinador.id_treinador, placares[i].id_treinador);
		int treinador_encontrado = busca_binaria(&chave_treinador, treinadores_idx, qtd_registros_treinadores, 
												sizeof(treinadores_index), qsort_treinadores_idx, false, 0);
		if(treinador_encontrado != -1 && treinadores_idx[treinador_encontrado].rrn != -1){
			// Se existe, recupero o registro e dou a recompensa
			Treinador t = recuperar_registro_treinador(treinadores_idx[treinador_encontrado].rrn);
			t.bolsobolas += premio;
			strcpy(t.premio, data_fim);
			escrever_registro_treinador(t, treinadores_idx[treinador_encontrado].rrn);
			if (i > 0) printf(ERRO_TREINADOR_REMOVIDO, premio, t.apelido, placares[i].vezes_mais_tempo, placares[i].vezes_mais_derrotados, placares[i].vezes_mais_dano);
			else printf(CONCEDER_PREMIO, t.apelido, placares[i].vezes_mais_tempo, placares[i].vezes_mais_derrotados, placares[i].vezes_mais_dano, premio);

			return;
		
		} 
	}

	printf(AVISO_NENHUM_REGISTRO_ENCONTRADO);
}

// ---------------- Busca pelo índice ----------------

void buscar_treinador_id_menu(char *id_treinador) {
	treinadores_index index;
	strcpy(index.id_treinador, id_treinador);
	int found = busca_binaria((void*)&index, treinadores_idx, qtd_registros_treinadores, sizeof(treinadores_index), qsort_treinadores_idx, true, 0);
	if(found == -1 || treinadores_idx[found].rrn < 0)
		printf(ERRO_REGISTRO_NAO_ENCONTRADO);
	else
		exibir_treinador(treinadores_idx[found].rrn);
}

void buscar_bolsomon_id_menu(char *id_bolsomon) {
	
	bolsomons_index chave_bolsomon;
	strcpy(chave_bolsomon.id_bolsomon, id_bolsomon);
	int bolsomon_encontrado = busca_binaria(&chave_bolsomon, bolsomons_idx, qtd_registros_bolsomons,
														sizeof(bolsomons_index), qsort_bolsomons_idx, true, 0);

	if(bolsomon_encontrado == -1 || bolsomons_idx[bolsomon_encontrado].rrn == -1){
		printf(ERRO_REGISTRO_NAO_ENCONTRADO);
		return;	
	}

	exibir_bolsomon(bolsomon_encontrado);
}

void buscar_batalha_id_menu(char *id_batalha) {

	batalhas_index chave_batalha;
	strcpy(chave_batalha.id_batalha, id_batalha);
	int batalha_encontrada = busca_binaria(&chave_batalha, batalhas_idx, qtd_registros_batalhas, sizeof(batalhas_index),
											qsort_batalhas_idx, true, 0);

	if(batalha_encontrada == -1 || batalhas_idx[batalha_encontrada].rrn == -1){
		printf(ERRO_REGISTRO_NAO_ENCONTRADO);
		return;
	}

	exibir_batalha(batalha_encontrada);	
}

// ---------------- Listagem dos registros ----------------

void listar_treinadores_id_menu() {
	if(!qtd_registros_treinadores){
		printf(AVISO_NENHUM_REGISTRO_ENCONTRADO);
        return;
    }
	for(unsigned int i = 0; i < qtd_registros_treinadores; i++)
		exibir_treinador(treinadores_idx[i].rrn);
}

void listar_treinadores_bolsomons_menu(char *bolsomon){

    int primeiro_indice;
    int total = 0;

    char nome[TAM_MAX_NOME_BOLSOMON];
    strcpy(nome, bolsomon);
    strupr(nome);

    // Primeiro Busco o primeiro indice do bolsomon na lista secundaria e verifico se o bolsomon existe e foi encontrado
    bool b_encontrado = inverted_list_secondary_search(&primeiro_indice, true, nome, &treinador_bolsomons_idx);

    if(!b_encontrado){
        printf(AVISO_NENHUM_REGISTRO_ENCONTRADO);
        return;
    }

    // Crio um vetor para alocar o id dos treinadores encontrados
    char treinadores[MAX_REGISTROS][TAM_CHAVE_TREINADOR_BOLSOMON_PRIMARIO_IDX];

    // Se existe, busco os ids dos treinadores
    total = inverted_list_primary_search(treinadores, true, primeiro_indice, NULL, &treinador_bolsomons_idx);
        
    // Verifico se algum treinador foi encontrado
    if(!total){
        printf(AVISO_NENHUM_REGISTRO_ENCONTRADO);
        return;
    }

    // Ordeno o id dos treinadores para mostrar o resultado ordenado
    qsort(treinadores, total, sizeof(treinadores[0]), qsort_inverted_list_primary_search);
    bool nenhum_encontrado = true;
    for(unsigned i = 0; i < total; i++){
        // Busco cada treinador no índice para recuperar seu registro
        treinadores_index chave_treinador;
        strncpy(chave_treinador.id_treinador, treinadores[i], TAM_CHAVE_TREINADOR_BOLSOMON_PRIMARIO_IDX);
		chave_treinador.id_treinador[TAM_CHAVE_TREINADOR_BOLSOMON_PRIMARIO_IDX] = '\0';
        int treinador_encontrado = busca_binaria(&chave_treinador, treinadores_idx, qtd_registros_treinadores,
                                                sizeof(treinadores_index), qsort_treinadores_idx, false, 0);
        
        // Verifico se o treinador existe ou se não foi removido
        if(treinador_encontrado != -1 && treinadores_idx[treinador_encontrado].rrn != -1){
            // Se o registro existe mostro suas informações
            exibir_treinador(treinadores_idx[treinador_encontrado].rrn);
            nenhum_encontrado = false;
        }
    }

    if(nenhum_encontrado) printf(AVISO_NENHUM_REGISTRO_ENCONTRADO);
}

void listar_bolsomons_compra_menu(char *id_treinador) {

	// Verifico se há registros no arquivo
	if(!qtd_registros_bolsomons){
		printf(ERRO_ARQUIVO_VAZIO);
		return;
	}

	// Busco o treinador para pegar sua qtd de bolsobolas
	treinadores_index chave_treinador;
	strcpy(chave_treinador.id_treinador, id_treinador);
	int treinador_encontrado = busca_binaria(&chave_treinador, treinadores_idx, qtd_registros_treinadores, 
											sizeof(treinadores_index), qsort_treinadores_idx, false, 0);
	if (treinador_encontrado == -1 || treinadores_idx[treinador_encontrado].rrn == -1) {
		printf(ERRO_REGISTRO_NAO_ENCONTRADO);
		return;
	}

	Treinador t = recuperar_registro_treinador(treinadores_idx[treinador_encontrado].rrn);

	// Como o idx de preço está ordenado exatamente pelo preço, percorro ele até achar o primeiro bolsomon fora do limite
	for(unsigned i = 0; i < qtd_registros_bolsomons; i++){
		// Verifico se o treinador possui as bolsobolas para comprar o bolsomon
		if(preco_bolsomon_idx[i].preco_bolsobolas > t.bolsobolas) break;

		bolsomons_index chave_bolsomon; 
		strcpy(chave_bolsomon.id_bolsomon, preco_bolsomon_idx[i].id_bolsomon);
		int bolsomon_encontrado = busca_binaria(&chave_bolsomon, bolsomons_idx, qtd_registros_bolsomons,
												sizeof(bolsomons_index), qsort_bolsomons_idx, false, 0);
		
		// Mostro os dados do bolsomon 
		if(bolsomon_encontrado != -1) exibir_bolsomon(bolsomons_idx[bolsomon_encontrado].rrn);
	}
}

void listar_batalhas_periodo_menu(char *data_inicio, char *data_fim) {

	data_index chave_data;
	strcpy(chave_data.inicio, data_inicio);
	strcpy(chave_data.id_batalha, "");
	int data_encontrada = busca_binaria(&chave_data, data_idx, qtd_registros_batalhas,
										sizeof(data_index), qsort_data_idx, true, 1);
	
	int c = 0;
	for(unsigned i = data_encontrada; i < qtd_registros_batalhas; i++){
		if(strcmp(data_idx[i].inicio, data_fim) > 0) break;

		batalhas_index ba_idx;
		strcpy(ba_idx.id_batalha, data_idx[i].id_batalha);
		int ba_encontrada = busca_binaria(&ba_idx, batalhas_idx, qtd_registros_batalhas,
										sizeof(batalhas_index), qsort_batalhas_idx, true, 0);
		if(ba_encontrada != -1){
			exibir_batalha(batalhas_idx[ba_encontrada].rrn);
			c++;
		} else printf(ERRO_REGISTRO_NAO_ENCONTRADO);
	}

	if(!c) printf(AVISO_NENHUM_REGISTRO_ENCONTRADO);
}

// ---------------- Exibição dos arquivos de dados ----------------

void imprimir_arquivo_treinadores_menu() {
	if(!qtd_registros_treinadores){
		printf(ERRO_ARQUIVO_VAZIO);
        return;
    }
	printf("%s\n", ARQUIVO_TREINADORES);
}

void imprimir_arquivo_bolsomons_menu() {
	if(!qtd_registros_bolsomons){
		printf(ERRO_ARQUIVO_VAZIO);
        return;
    }
	printf("%s\n", ARQUIVO_BOLSOMONS);
}

void imprimir_arquivo_batalhas_menu() {

	if(!qtd_registros_batalhas){
		printf(ERRO_ARQUIVO_VAZIO);
		return;
	}

	printf("%s\n",ARQUIVO_BATALHAS);
}

void imprimir_arquivo_resultados_menu() {
	if(!qtd_registros_resultados){
		printf(ERRO_ARQUIVO_VAZIO);
		return;
	}

	printf("%s\n",ARQUIVO_RESULTADOS);
}

void imprimir_arquivo_treinador_possui_bolsomon_menu() {
	if(!qtd_registros_treinador_possui_bolsomon){
		printf(ERRO_ARQUIVO_VAZIO);
		return;
	}

	printf("%s\n",ARQUIVO_TREINADOR_POSSUI_BOLSOMON);
}

// ---------------- Exibição dos índices ----------------

void imprimir_treinadores_idx_menu() {
	if(!treinadores_idx || !qtd_registros_treinadores){
		printf(ERRO_ARQUIVO_VAZIO);
        return;
    }
	for(unsigned i = 0; i < qtd_registros_treinadores; i++)
		printf("%s, %d\n", treinadores_idx[i].id_treinador, treinadores_idx[i].rrn);
}

void imprimir_bolsomons_idx_menu() {

	// Verifico se o arquivo está vazio
	if(!bolsomons_idx || !qtd_registros_bolsomons){
		printf(ERRO_ARQUIVO_VAZIO);
        return;
    }
	// Percorro todo o índice para imprimir os dados 
	for(unsigned i = 0; i < qtd_registros_bolsomons; i++)
		printf("%s, %d\n", bolsomons_idx[i].id_bolsomon, bolsomons_idx[i].rrn);
}

void imprimir_batalhas_idx_menu() {

	if(!batalhas_idx || !qtd_registros_batalhas){
		printf(ERRO_ARQUIVO_VAZIO);
		return;
	}

	for(unsigned i = 0; i < qtd_registros_batalhas; i++)
		printf("%s, %d\n", batalhas_idx[i].id_batalha, batalhas_idx[i].rrn);
}

void imprimir_resultados_idx_menu() {
	if(!resultados_idx || !qtd_registros_resultados){
		printf(ERRO_ARQUIVO_VAZIO);
		return;
	}

	for(unsigned i = 0; i < qtd_registros_resultados; i++)
		printf("%s, %s, %d\n", resultados_idx[i].id_treinador, resultados_idx[i].id_batalha, resultados_idx[i].rrn);
}

void imprimir_treinador_possui_bolsomon_idx_menu() {
	if(!treinador_possui_bolsomon_idx || !qtd_registros_treinador_possui_bolsomon){
		printf(ERRO_ARQUIVO_VAZIO);
		return;
	}

	for(unsigned i = 0; i < qtd_registros_treinador_possui_bolsomon; i++)
		printf("%s, %s, %d\n", treinador_possui_bolsomon_idx[i].id_treinador, treinador_possui_bolsomon_idx[i].id_bolsomon, treinador_possui_bolsomon_idx[i].rrn);
}

void imprimir_preco_bolsomon_idx_menu() {
	if(!preco_bolsomon_idx || !qtd_registros_bolsomons){
		printf(ERRO_ARQUIVO_VAZIO);
        return;
    }
	for(unsigned i = 0; i < qtd_registros_bolsomons; i++)
	printf("%.2lf, %s\n", preco_bolsomon_idx[i].preco_bolsobolas, preco_bolsomon_idx[i].id_bolsomon);
}

void imprimir_data_idx_menu() {
	if(!data_idx || !qtd_registros_batalhas){
		printf(ERRO_ARQUIVO_VAZIO);
        return;
    }
	for(unsigned i = 0; i < qtd_registros_batalhas; i++)
		printf("%s, %s\n", data_idx[i].id_batalha, data_idx[i].inicio);
}

void imprimir_treinador_bolsomons_secundario_idx_menu() {
	
	if(!treinador_bolsomons_idx.qtd_registros_secundario) {
		printf(ERRO_ARQUIVO_VAZIO);
		return;
	}

	for(unsigned i = 0; i < treinador_bolsomons_idx.qtd_registros_secundario; i++)
		printf("%s, %d\n", treinador_bolsomons_idx.treinador_bolsomons_secundario_idx[i].chave_secundaria,
							treinador_bolsomons_idx.treinador_bolsomons_secundario_idx[i].primeiro_indice);
	
}

void imprimir_treinador_bolsomons_primario_idx_menu() {
	if(!treinador_bolsomons_idx.qtd_registros_primario) {
		printf(ERRO_ARQUIVO_VAZIO);
		return;
	}

	for(unsigned i = 0; i < treinador_bolsomons_idx.qtd_registros_primario; i++)
		printf("%s, %d\n", treinador_bolsomons_idx.treinador_bolsomons_primario_idx[i].chave_primaria,
							treinador_bolsomons_idx.treinador_bolsomons_primario_idx[i].proximo_indice);
	
}

// ---------------- Liberação de espaço e memória ----------------

void liberar_espaco_menu() {

	// Buffer para pegar somente os registros válidos
	char *buffer = (char*)malloc(MAX_REGISTROS * sizeof(char));
	if(!buffer) { printf(ERRO_MEMORIA_INSUFICIENTE); return; }
	
	// Variáveis auxiliares para atualizar o rrn e quantidade de registros
	unsigned novo_rrn = 0;
	
	// Preencho o buffer somente com registros não removidos
	for(unsigned i = 0; i < qtd_registros_treinadores; i++){
		char *reg_ptr = ARQUIVO_TREINADORES + i * TAM_REGISTRO_TREINADOR;
		if(strncmp(reg_ptr, "*|", 2) != 0){
			memcpy(buffer + novo_rrn * TAM_REGISTRO_TREINADOR, reg_ptr, TAM_REGISTRO_TREINADOR);
			novo_rrn++;
		}
	}

	// Atualizo o arquivo com o buffer correto
	memset(ARQUIVO_TREINADORES, 0, qtd_registros_treinadores *TAM_REGISTRO_TREINADOR);
	memcpy(ARQUIVO_TREINADORES, buffer, novo_rrn * TAM_REGISTRO_TREINADOR);

	// Libero o buffer 
	free(buffer);

	// Atualizo a quantidade de treinadores
	qtd_registros_treinadores = novo_rrn;

	// Atualizo o índice
	if(!treinadores_idx)
		treinadores_idx = malloc(MAX_REGISTROS * sizeof(treinadores_index));

	if(!treinadores_idx) {
		printf(ERRO_MEMORIA_INSUFICIENTE);
		exit(1);
	}

	for(unsigned i = 0; i < qtd_registros_treinadores; i++) {
		Treinador t = recuperar_registro_treinador(i);

		treinadores_idx[i].rrn = (strncmp(t.id_treinador, "*|", 2)) ? i : -1;

		strcpy(treinadores_idx[i].id_treinador, t.id_treinador);
	}

	// Reordeno o índice
	qsort(treinadores_idx, qtd_registros_treinadores, sizeof(treinadores_index), qsort_treinadores_idx);

	// OBS: chamo criar_treinadores_idx pois a função tem um printf no final que afeta na saída 
	printf(SUCESSO);
}

void liberar_memoria_menu() {
	
	// Libero a memória alocada para os índices 
	if(treinadores_idx) free(treinadores_idx);
	
	if(bolsomons_idx) free(bolsomons_idx);
	
	if(batalhas_idx) free(batalhas_idx);
	
	if(resultados_idx) free(resultados_idx);
	
	if(treinador_possui_bolsomon_idx) free(treinador_possui_bolsomon_idx);
	
	if(preco_bolsomon_idx) free(preco_bolsomon_idx);
	
	if(data_idx) free(data_idx);
	
	if(treinador_bolsomons_idx.treinador_bolsomons_secundario_idx) 
		free(treinador_bolsomons_idx.treinador_bolsomons_secundario_idx);
	if(treinador_bolsomons_idx.treinador_bolsomons_primario_idx) 
		free(treinador_bolsomons_idx.treinador_bolsomons_primario_idx);	

}

// ---------------- Manipulação da lista invertida ----------------

void inverted_list_insert(char *chave_secundaria, char *chave_primaria, inverted_list *t) {

	treinador_bolsomons_secundario_index *sec_idx = t->treinador_bolsomons_secundario_idx;

	chave_secundaria = strupr(chave_secundaria);

	unsigned int *sec_count = &t->qtd_registros_secundario;

	// Verifica se a chave secundária já existe no índice secundário
	int sec_idx_pos = -1;
	for(unsigned i = 0; i < *sec_count; i++) {
		if(strcmp(sec_idx[i].chave_secundaria, chave_secundaria) == 0) {
			sec_idx_pos = i;
			break;
		}
	}

	// Se a chave secundária não existe, cria uma nova entrada no índice secundário
	if(sec_idx_pos == -1){
		// Faço a inserção da nova chave em ambos os índices
		strcpy(t->treinador_bolsomons_secundario_idx[t->qtd_registros_secundario].chave_secundaria, chave_secundaria);

		t->treinador_bolsomons_secundario_idx[t->qtd_registros_secundario].primeiro_indice = t->qtd_registros_primario;
        
        strcpy(t->treinador_bolsomons_primario_idx[t->qtd_registros_primario].chave_primaria, chave_primaria);
        t->treinador_bolsomons_primario_idx[t->qtd_registros_primario].proximo_indice = -1;
        
        t->qtd_registros_secundario++;
        t->qtd_registros_primario++;
	} else{
		// Se a chave já está no índice, adiciono ao próximo índice
		int inicio_lista = t->treinador_bolsomons_secundario_idx[sec_idx_pos].primeiro_indice;
		int indice_final;

		// Pego o último indice na lista primária 
		inverted_list_primary_search(NULL, false, inicio_lista, &indice_final, t);

		strcpy(t->treinador_bolsomons_primario_idx[t->qtd_registros_primario].chave_primaria, chave_primaria);
        t->treinador_bolsomons_primario_idx[t->qtd_registros_primario].proximo_indice = -1;
        
        t->treinador_bolsomons_primario_idx[indice_final].proximo_indice = t->qtd_registros_primario;
        t->qtd_registros_primario++;
	}
}

bool inverted_list_secondary_search(int *result, bool exibir_caminho, char *chave_secundaria, inverted_list *t) {
	
	treinador_bolsomons_secundario_index t_sec;
	strcpy(t_sec.chave_secundaria,chave_secundaria);

	int encontrado = busca_binaria(&t_sec, treinador_bolsomons_idx.treinador_bolsomons_secundario_idx,
					treinador_bolsomons_idx.qtd_registros_secundario, 
					sizeof(treinador_bolsomons_secundario_index), qsort_treinador_bolsomons_secundario_idx,
					exibir_caminho == true, 0);
	
	if(encontrado != -1){
		if (result) *result = treinador_bolsomons_idx.treinador_bolsomons_secundario_idx[encontrado].primeiro_indice;
		return true;
	}

	return false;
}

int inverted_list_primary_search(char result[][TAM_CHAVE_TREINADOR_BOLSOMON_PRIMARIO_IDX], bool exibir_caminho, int indice, int *indice_final, inverted_list *t) {
	
	if(exibir_caminho) printf("%s", REGS_PERCORRIDOS);
	int c = 0;
	int atual = indice;
	while(atual != -1){
		if(exibir_caminho) printf(" %d", atual);
		
		if(result)
			strcpy(result[c], t->treinador_bolsomons_primario_idx[atual].chave_primaria);
		
		if(indice_final && t->treinador_bolsomons_primario_idx[atual].proximo_indice == -1)
			*indice_final = atual;	
		
		c++;
		atual = t->treinador_bolsomons_primario_idx[atual].proximo_indice;
	}

	if(exibir_caminho) printf("\n");
	return c;
}

// ---------------- Busca binária ----------------

int busca_binaria(const void *key, const void *base0, size_t nmemb, size_t size, int (*compar)(const void *, const void *), bool exibir_caminho, int retorno_se_nao_encontrado) {

	// Verifico se não está vazio
	if(nmemb == 0){
		if(exibir_caminho) printf("%s\n", REGS_PERCORRIDOS);
		return -1;
	}

	// Inicializo as variáveis auxiliares
	int inicio = 0, fim = nmemb - 1;
	if(exibir_caminho) printf(REGS_PERCORRIDOS);
	while(inicio <= fim){
		// Para escolher a posição mais a direita
		int meio = inicio + (fim - inicio + 1) / 2;
		if(exibir_caminho) printf(" %d", meio);
		
		// Pego o valor do meio
		const void *n = (const char *)base0 + meio * size;

		// Comparo o meio com a chave
		int cmp = compar(key, n);

		if(cmp == 0){
			if(exibir_caminho) printf("\n");
			return meio;
		}else if (cmp < 0) fim = meio - 1;
		else inicio = meio + 1;
	}
	
	if (exibir_caminho) printf("\n");
    if (retorno_se_nao_encontrado == 0) return -1;
    else if (retorno_se_nao_encontrado == -1) {
        if (fim < 0) return -1;
        return fim;
    }else if (retorno_se_nao_encontrado == 1) {
        if (inicio >= (int)nmemb) return (int)nmemb;
        return inicio;
    }
    return -1;
}

// ---------------- Funções SET de arquivos ----------------

void set_arquivo_treinadores(char* novos_dados, size_t tamanho){
    memcpy(ARQUIVO_TREINADORES, novos_dados, tamanho);
    qtd_registros_treinadores = tamanho / TAM_REGISTRO_TREINADOR;
}

void set_arquivo_bolsomons(char* novos_dados, size_t tamanho){
    memcpy(ARQUIVO_BOLSOMONS, novos_dados, tamanho);
    qtd_registros_bolsomons = tamanho / TAM_REGISTRO_BOLSOMON;
}

void set_arquivo_batalhas(char* novos_dados, size_t tamanho){
    memcpy(ARQUIVO_BATALHAS, novos_dados, tamanho);
    qtd_registros_batalhas = tamanho / TAM_REGISTRO_BATALHA;
}

void set_arquivo_resultados(char* novos_dados, size_t tamanho){
    memcpy(ARQUIVO_RESULTADOS, novos_dados, tamanho);
    qtd_registros_resultados = tamanho / TAM_REGISTRO_RESULTADO;
}

void set_arquivo_treinador_possui_bolsomon(char* novos_dados, size_t tamanho){
    memcpy(ARQUIVO_TREINADOR_POSSUI_BOLSOMON, novos_dados, tamanho);
    qtd_registros_treinador_possui_bolsomon = tamanho / TAM_REGISTRO_TREINADOR_POSSUI_BOLSOMON;
}

// ---------------- Implementação das suas próprias funções auxiliares ----------------