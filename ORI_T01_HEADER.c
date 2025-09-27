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
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "qsort_info_treinador()");
	return 0;
}

/* Função de comparação entre chaves do índice secundário de treinador_bolsomons_secundario_idx */
int qsort_treinador_bolsomons_secundario_idx(const void *a, const void *b) {
	return strcmp(((treinador_bolsomons_secundario_index*)a)->chave_secundaria, ((treinador_bolsomons_secundario_index*)b)->chave_secundaria);
}

/* Função de comparação entre chaves do índice primário da lista invertida */
int qsort_inverted_list_primary_search(const void *a, const void *b) {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "qsort_inverted_list_primary_search()");
	return 0;
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
	printf(INDICE_CRIADO, "batalhas_idx()");
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
	printf(INDICE_CRIADO, "resultados_idx()");
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
	printf(INDICE_CRIADO, "treinador_possui_bolsomon_idx()");
}

void criar_preco_bolsomon_idx() {
	if(!preco_bolsomon_idx) preco_bolsomon_idx = malloc(MAX_REGISTROS * sizeof(preco_bolsomon_index));

	if(!preco_bolsomon_idx) {
		printf(ERRO_MEMORIA_INSUFICIENTE);
		exit(1);
	}

	for(unsigned i = 0; i < qtd_registros_bolsomons; i++) {
	
		TreinadorPossuiBolsomon tpb = recuperar_registro_treinador_possui_bolsomon(i);

		treinador_possui_bolsomon_idx[i].rrn = i;
		strcpy(treinador_possui_bolsomon_idx[i].id_treinador, tpb.id_treinador);
		strcpy(treinador_possui_bolsomon_idx[i].id_bolsomon, tpb.id_bolsomon);
	}

	qsort(treinador_possui_bolsomon_idx, qtd_registros_treinador_possui_bolsomon, sizeof(treinador_possui_bolsomon_index), qsort_treinador_possui_bolsomon_idx);
	printf(INDICE_CRIADO, "treinador_possui_bolsomon_idx()");
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
	printf(INDICE_CRIADO, "data_idx()");
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

    for(unsigned i = 0; i < qtd_registros_treinador_possui_bolsomon; i++){
		// Recuperando o registro do treinador_possui_bolsomon
		treinador_possui_bolsomon_index tpb_idx = treinador_possui_bolsomon_idx[i];

		// Busco o rrn do bolsomon no índice de bolsomons
		bolsomons_index chave_busca;
		strcpy(chave_busca.id_bolsomon, tpb_idx.id_bolsomon);
		bolsomons_index *b_idx = bsearch(&chave_busca, bolsomons_idx, qtd_registros_bolsomons, sizeof(bolsomons_index), qsort_bolsomons_idx);

		// Se o bolsomon existir, recupero o registro para pegar o nome e insiro na lista invertida
		if(b_idx && b_idx->rrn != -1){
			Bolsomon b = recuperar_registro_bolsomon(b_idx->rrn);
			inverted_list_insert(b.nome, tpb_idx.id_treinador, &treinador_bolsomons_idx);
		}

	}

	// Por fim, ordeno o índice secundário
	qsort(treinador_bolsomons_idx.treinador_bolsomons_secundario_idx, treinador_bolsomons_idx.qtd_registros_secundario, 
          sizeof(treinador_bolsomons_secundario_index), 
          qsort_treinador_bolsomons_secundario_idx);


    printf(INDICE_CRIADO, "treinador_bolsomons_primario_idx");
    printf(INDICE_CRIADO, "treinador_bolsomons_secundario_idx");
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

/*
Bolsomon b = {
	char id_bolsomon[TAM_ID_BOLSOMON];
	char nome[TAM_MAX_NOME_BOLSOMON];
	char habilidade[TAM_MAX_HABILIDADE_BOLSOMON];
	double preco_bolsobolas;
};*/

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

	printf("escrever_registro_bolsomon() FEITO\n");
}

/*
Batalha ba{
	char id_batalha[TAM_ID_BATALHA];
	char inicio[TAM_INICIO_BATALHA];
	char duracao[TAM_DURACAO_BATALHA];
	char arena[TAM_ARENA_BATALHA];
};
*/


void escrever_registro_batalha(Batalha b, int rrn) {

	// Mesma rotina de escrever_registro_bolsomon(), adaptada para a struct Batalha

	char buffer[TAM_REGISTRO_BATALHA + 1];
	buffer[0] = '\0';

	strcat(buffer, b.id_batalha); strcat(buffer, ";");
	strcat(buffer, b.inicio); strcat(buffer, ";");
	strcat(buffer, b.duracao); strcat(buffer, ";");
	strcat(buffer, b.arena); strcat(buffer, ";");

	strpadright(buffer, '#', TAM_REGISTRO_BATALHA);

	strncpy(ARQUIVO_BATALHAS + rrn * TAM_REGISTRO_BATALHA, buffer, TAM_REGISTRO_BATALHA);

	printf("escrever_registro_batalha() FEITO\n");
}

/*
Resultado r = {
	char id_treinador[TAM_ID_TREINADOR];
	char id_batalha[TAM_ID_BATALHA];
	char id_bolsomon[TAM_ID_BOLSOMON];
	bool foi_maior_duracao;
	bool foi_mais_derrotas;
	bool foi_mais_dano;
}
*/

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

	strncpy(ARQUIVO_RESULTADOS + rrn*TAM_ARQUIVO_RESULTADOS, buffer, TAM_REGISTRO_RESULTADO);
	printf("escrever_registro_resultado() FEITO\n");
}

/*
TreinadorPossuiBolsomon tpb = {
	char id_treinador[TAM_ID_TREINADOR];
	char id_bolsomon[TAM_ID_BOLSOMON];
};*/

void escrever_registro_treinador_possui_bolsomon(TreinadorPossuiBolsomon tpb, int rrn) {

	char buffer[TAM_REGISTRO_TREINADOR_POSSUI_BOLSOMON + 1];
	buffer[0] = '\0';

	sprintf(buffer, "%.11s%.3s", tpb.id_treinador, tpb.id_bolsomon);

	strncpy(ARQUIVO_TREINADOR_POSSUI_BOLSOMON + rrn * TAM_REGISTRO_TREINADOR_POSSUI_BOLSOMON, buffer, TAM_REGISTRO_TREINADOR_POSSUI_BOLSOMON);
	printf("escrever_registro_treinador_possui_bolsomon() FEITO");
}

// ---------------- Exibição dos registros ----------------

bool exibir_treinador(int rrn) {
	if(rrn < 0) return false;
    
	Treinador t = recuperar_registro_treinador(rrn);
    
	printf("%s, %s, %s, %s, %.2lf\n", t.id_treinador, t.apelido, t.cadastro, t.premio, t.bolsobolas);
    
	return true;
}

bool exibir_bolsomon(int rrn) {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "exibir_bolsomon()");
	return false;
}

bool exibir_batalha(int rrn) {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "exibir_batalha()");
	return false;
}

// ---------------- Funções principais ----------------

void cadastrar_treinador_menu(char *id_treinador, char *apelido) {
	
	// Primeiro, busco o rrn do treinador no índice para verificar se ele já existe
	treinadores_index chave_busca;
	strcpy(chave_busca.id_treinador, id_treinador);
	treinadores_index *encontrado = bsearch(&chave_busca, treinadores_idx, qtd_registros_treinadores, sizeof(treinadores_index), qsort_treinadores_idx);
	
	if(encontrado && encontrado->rrn != -1){
		// Se o treinador já existir, exibo a mensagem de erro e retorno
		printf(ERRO_PK_REPETIDA, id_treinador);
		return;
	}

	// Se o treinador não existir, crio a struct Treinador com os dados fornecidos
	Treinador novo_treinador;
	strcpy(novo_treinador.id_treinador, id_treinador);
	strcpy(novo_treinador.apelido, apelido);
	novo_treinador.bolsobolas = 0.0;
	strcpy(novo_treinador.premio, "000000000000"); // Inicializo o campo premio como string vazia
	current_date(novo_treinador.cadastro); // Preencho o campo cadastro com a data atual

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
	treinadores_index *encontrado = bsearch(&chave_busca, treinadores_idx, qtd_registros_treinadores, 
		sizeof(treinadores_index), qsort_treinadores_idx);

	if(!encontrado || encontrado->rrn == -1){
		// Se o treinador não existir, exibo a mensagem de erro e retorno
		printf(ERRO_REGISTRO_NAO_ENCONTRADO);
		return;
	}

	// Se o treinador existir, marco o registro como removido no arquivo de dados
	strncpy(ARQUIVO_TREINADORES + (encontrado->rrn * TAM_REGISTRO_TREINADOR), "*|", 2);

	// Atualizo o índice de treinadores marcando o RRN como -1 (removido)
	encontrado->rrn = -1;

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
	treinadores_index *encontrado = bsearch(&chave_busca, treinadores_idx, 
		qtd_registros_treinadores, sizeof(treinadores_index), qsort_treinadores_idx);
	
	if(!encontrado || encontrado->rrn == -1){
		// Se o treinador não existir, exibo a mensagem de erro e retorno
		printf(ERRO_REGISTRO_NAO_ENCONTRADO);
		return;
	}

	// Se o treinador existir, recupero o registro e atualizo o campo de bolsobolas
	Treinador t = recuperar_registro_treinador(encontrado->rrn);
	t.bolsobolas += valor;

	// Escrevo o registro atualizado de volta no arquivo de dados
	escrever_registro_treinador(t, encontrado->rrn);

	if(flag) printf(SUCESSO);	
}

void cadastrar_bolsomon_menu(char *nome, char *habilidade, double preco) {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "cadastrar_bolsomon_menu()");
}

void comprar_bolsomon_menu(char *id_treinador, char *id_bolsomon) {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "comprar_bolsomon_menu()");
}

void executar_batalha_menu(char *inicio, char *duracao, char *arena, char *id_treinadores, char *bolsomons_treinadores, char *duracoes_treinadores, char *eliminacoes_treinadores, char *danos_causados_treinadores) {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "executar_batalha_menu()");
}

void recompensar_campeao_menu(char *data_inicio, char *data_fim, double premio) {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "recompensar_campeao_menu()");
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
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "buscar_bolsomon_id_menu()");
}

void buscar_batalha_id_menu(char *id_batalha) {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "buscar_batalha_id_menu()");
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
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "listar_treinadores_bolsomons_menu()");
}

void listar_bolsomons_compra_menu(char *id_treinador) {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "listar_bolsomons_compra_menu()");
}

void listar_batalhas_periodo_menu(char *data_inicio, char *data_fim) {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "listar_batalhas_periodo_menu()");
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
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "imprimir_arquivo_bolsomons_menu()");
}

void imprimir_arquivo_batalhas_menu() {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "imprimir_arquivo_batalhas_menu()");
}

void imprimir_arquivo_resultados_menu() {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "imprimir_arquivo_resultados_menu()");
}

void imprimir_arquivo_treinador_possui_bolsomon_menu() {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "imprimir_arquivo_treinador_possui_bolsomon_menu()");
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
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "imprimir_bolsomons_idx_menu()");
}

void imprimir_batalhas_idx_menu() {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "imprimir_batalhas_idx_menu()");
}

void imprimir_resultados_idx_menu() {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "imprimir_resultados_idx_menu()");
}

void imprimir_treinador_possui_bolsomon_idx_menu() {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "imprimir_treinador_possui_bolsomon_idx_menu()");
}

void imprimir_preco_bolsomon_idx_menu() {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "imprimir_preco_bolsomon_idx_menu()");
}

void imprimir_data_idx_menu() {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "imprimir_data_idx_menu()");
}

void imprimir_treinador_bolsomons_secundario_idx_menu() {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "imprimir_treinador_bolsomons_secundario_idx_menu()");
}

void imprimir_treinador_bolsomons_primario_idx_menu() {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "imprimir_treinador_bolsomons_primario_idx_menu()");
}

// ---------------- Liberação de espaço e memória ----------------

void liberar_espaco_menu() {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "liberar_espaco_menu()");
}

void liberar_memoria_menu() {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "liberar_memoria_menu()");
}

// ---------------- Manipulação da lista invertida ----------------

void inverted_list_insert(char *chave_secundaria, char *chave_primaria, inverted_list *t) {
	
	treinador_bolsomons_secundario_index *sec_idx = t->treinador_bolsomons_secundario_idx;
	treinador_bolsomons_primario_index *prim_idx = t->treinador_bolsomons_primario_idx;

	unsigned int *prim_count = &t->qtd_registros_primario;
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
	if(sec_idx_pos == -1) {
		// Atualiza a chave secundária e incrementa o contador do índice secundário
		sec_idx_pos = (*sec_count)++;
		strcpy(sec_idx[sec_idx_pos].chave_secundaria, chave_secundaria);

		// Inicializa o índice primário associado
		sec_idx[sec_idx_pos].primeiro_indice = *prim_count;

		// Atualiza o índice primário
		strcpy(prim_idx[*prim_count].chave_primaria, chave_primaria);
		prim_idx[*prim_count].proximo_indice = -1; // Como é o começo da lista, não há próximo
		(*prim_count)++;
	} else{
		// Se a chave secundária já existe, adiciona a chave primária ao final da lista ligada
		// Como a ordem do índice primário é irrelevante, adicionamos no começo da lista

		int current_index = sec_idx[sec_idx_pos].primeiro_indice;
		int novo_index = (*prim_count)++;

		strcpy(prim_idx[novo_index].chave_primaria, chave_primaria);
		prim_idx[novo_index].proximo_indice = current_index; // O próximo é o antigo primeiro índice

		sec_idx[sec_idx_pos].primeiro_indice = novo_index; // Atualiza o primeiro índice para o novo
	}
	}

bool inverted_list_secondary_search(int *result, bool exibir_caminho, char *chave_secundaria, inverted_list *t) {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "inverted_list_secondary_search()");
	return false;
}

int inverted_list_primary_search(char result[][TAM_CHAVE_TREINADOR_BOLSOMON_PRIMARIO_IDX], bool exibir_caminho, int indice, int *indice_final, inverted_list *t) {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "inverted_list_primary_search()");
	return 0;
}

// ---------------- Busca binária ----------------

int busca_binaria(const void *key, const void *base0, size_t nmemb, size_t size, int (*compar)(const void *, const void *), bool exibir_caminho, int retorno_se_nao_encontrado) {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "busca_binaria()");
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

