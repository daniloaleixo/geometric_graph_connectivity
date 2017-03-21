/* ***********************************************

Nome: Danilo Aleixo Gomes de Souza
N USP: 7972370

Prof: Yoshiharu Kohayakawa

ep1.c
Calculando a densidade normalizada de um grafo

************************************************ */




#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>

/* Conexao onde guardaremos informaçoes das arestas */
typedef struct conexao
{
    float distancia;
    struct conexao *proxConexao;
    int numeroVertice;
} Conexao;

/* Ponto onde guardaremos informaçoes dos vertices */
typedef struct ponto
{
    float x;
    float y;
    int nVertice;
    Conexao *conectadoCom;
} Ponto;


#define TRUE 1
#define FALSE 0


/*declaraçao de funçoes utilizadas */
float densidadeNormalizada(Ponto *grafo, int n, int m, int seed, int queroVerLimiar, int queroVerVerts);
int conexo(Ponto *grafo, int n);
float extraiNumero(char *string);
float getFloat();
float distanciaPontos(Ponto grafo[], int i, int j);
float menorDistanciaBusca(Ponto *grafo, int n);
void quicksort(Ponto grafo[], int p, int r);
int separa(Ponto grafo[], int p, int r );
int recursao(Ponto *grafo, int n, int *jaVisitei, int pontoAnalisado, int *cont);
void montaGrafo(Ponto *grafo, int N, float d, int queroVerVerts);
Ponto *geraPontosAleatorios(Ponto *grafo, int seed, int N, int C);
void *mallocSafe(size_t n);

/* ---------------------------- */


int main(int argc, char *argv[])
{
    Ponto *grafo; /* vetor de Pontos q guardarao as informaçoes do grafo */
    int N = 0; /* numero de pontos do grafo */
    int M = 0; /* numero de istancias que geram a densidade */
    float d = 0; /* distancia entre os pontos */
    int C = FALSE; /* dar os vertices pela linha de comando */
    int queroVerVerts = FALSE; /* se o usuario entrar com -v queroVerVerts = True */
    int queroVerLimiar = FALSE; /* imprime os limiares de conexidade */
    int D = FALSE;
    int seed = 1; /* inicializamos a semente com 1 */
    int grafoConexo;
    float densNormalizada;
    int i;


    int numeroEntradas = argc - 1;

    /* Vamos ler as entradas dada pela linha de comando */
    while (numeroEntradas > 0)
    {
        switch(argv[numeroEntradas][1])
        {
        case 'M':
            M = (int) extraiNumero(argv[numeroEntradas]);
            break;

        case 'N':
            N = (int) extraiNumero(argv[numeroEntradas]);
            break;

        case 'd':
            d =  extraiNumero(argv[numeroEntradas]);
            break;

        case 's':
            seed =  (int)extraiNumero(argv[numeroEntradas]);
            break;

        case 'v':
            queroVerVerts = TRUE;
            break;

        case 'V':
            queroVerLimiar = TRUE;
            break;

        case 'C':
            C = TRUE;
            break;

        case 'D':
            D = TRUE;
            break;

        default:
            fprintf(stderr, "Entrada Invalida \n"
                    "As possiveis entradas sao: \n"
                    "\n"
                    "-M  para denotar o numero de instancias para a densidade normalizada \n"
                    "-N  para denotar o numero de pontos do grafo \n"
                    "-d para informar a distancia minima entre dois vertices que forma uma aresta  \n"
                    "-s para mudar a semente da funcao que gera as coordenadas randomicas \n"
                    "-v para mostrar as coordenadas dos vertices ou para mostrar os limiares de conexividade das M instancias\n"
                    "-V para mostrar as coordenadas dos vertices do M grafos gerados \n"
                    "-C para entrar com as coordenadas dos vertices pela linha de comando para testar sua conexividade \n"
                    "-D para entrar com as coordenadas dos vertices pela linha de comando e calcular a menor distancia que torna o grafo conexo \n "
                    "\n");
            exit(-1);
        }

        numeroEntradas--;
    }

    /* vamos testar a conectividade do grafo */
    if(d != 0)
    {
        if(C == TRUE)
        {
            printf("Digite o numero de pontos: \n");
            scanf("%d", &N);
        }

        grafo = geraPontosAleatorios(grafo, seed, N, C);
        montaGrafo(grafo, N, d, queroVerVerts);
        grafoConexo = conexo(grafo, N);
        if(grafoConexo == FALSE) printf("O grafo nao e conexo\n");
        else printf("O grafo e conexo\n");
    }

    /* testamos o limiar de conexidade*/
    if(M != 0)
    {
        i = queroVerLimiar;
        queroVerLimiar = queroVerVerts;
        queroVerVerts = i;

        densNormalizada = densidadeNormalizada(grafo, N, M, seed,queroVerLimiar, queroVerVerts);

        printf("A densidade normalizada e %f\n", densNormalizada);

    }

    /* caso queiramos encontrar a menor distancia de um grafo passado pela linha de comando */
    if(D == TRUE)
    {

        printf("Digite o numero de pontos: \n");
        scanf("%d", &N);

        grafo = geraPontosAleatorios(grafo, seed, N, 1);

        densNormalizada = menorDistanciaBusca(grafo, N);
        printf("A menor distancia que torna o grafo conexo e %f\n", densNormalizada);

    }

    return 0;

}

/* ________________________FUNCOES AUXILIARES ___________________________ */


/*
  mallocSafe: testa o ponteiro devolvido por malloc
 */
void * mallocSafe (size_t n)
{
    void * pt;
    pt = malloc(n);
    if (pt == NULL)
    {
        printf("ERRO na alocacao de memoria.\n\n");
        exit(-1);
    }
    return pt;
}


/* verifica se o grafo e conexo com N vertices*/
int conexo(Ponto *grafo, int n)
{
    int jaVisitei[n];
    int i;
    int pontoAnalisado = 0;
    for(i = 0; i < n; i++) jaVisitei[i] = 0; /* resetamos o vetor */
    int cont = 0;

    return recursao(grafo, n, jaVisitei, pontoAnalisado, &cont);

}



/* calcumos a densidade normalzada de N vertices para M instancias */
float densidadeNormalizada(Ponto *grafo, int n, int m, int seed, int queroVerLimiar, int queroVerVerts)
{
    float menorDist;
    float limiarConect;
    float mediaLimiar = 0;
    int loop = m;
    int i;
    Ponto *copiaGrafo = grafo;

    /* fazemos o procedimento M vezes */
    while(loop > 0)
    {
        copiaGrafo = geraPontosAleatorios(copiaGrafo, seed++, n, 0); /* geramos o grafo */

        if(queroVerVerts == TRUE)
        {
            for(i = 0; i < n; i++)
            {
                printf("ponto: %d -- x: %g -- y: %g\n", copiaGrafo[i].nVertice, copiaGrafo[i].x, copiaGrafo[i].y);
            }
            printf("\n");
        }

        /* calcula-se a menor Distancia */
        menorDist = menorDistanciaBusca(copiaGrafo, n);
        limiarConect = menorDist * menorDist * (n / log(n)); /* calcula-se o limiar de conectividade */
        if(queroVerLimiar == TRUE) printf("O limiar de conectividade e %f\n", limiarConect);
        mediaLimiar += limiarConect;
        loop--;
        free(copiaGrafo);

    }

    return mediaLimiar / m;
}

/* le um float da linha de comando */
float getFloat()
{
    float p = -1;
    scanf("%f", &p);
    return p;
}

/* recebe a string dada na linha de comando de programa e retorna o numero ligado a ela */
float extraiNumero(char *string)
{
    int i;
    int tamanhoString = strlen(string);
    char numero[tamanhoString - 1];
    for(i = 2; i < tamanhoString; i++)
    {
        numero[i - 2] = string[i];
    }
    numero[tamanhoString - 2] = '\0';
    return atof(numero);
}

/* Dados dois pontos, a funçao calcula a distancia entre eles */
float distanciaPontos(Ponto *grafo, int i, int j)
{
    return sqrt((grafo[i].x - grafo[j].x) * (grafo[i].x - grafo[j].x) + (grafo[i].y - grafo[j].y) * (grafo[i].y - grafo[j].y));
}

/* usamos a funçao separa baseada no algortimo do professor Paulo Feofiloff*/
int separa(Ponto grafo[], int p, int r )
{
    float c = grafo[p].x, t;
    float yJ = grafo[p].y;
    float y;

    int i = p + 1, j = r;
    while(i <= j)
    {
        if(grafo[i].x <= c) ++i;
        else if(c < grafo[j].x) --j;
        else
        {
            /* fazemos as trocas de variaveis neste ponto */
            t = grafo[i].x;
            grafo[i].x = grafo[j].x;
            grafo[j].x = t;

            y = grafo[i].y;
            grafo[i].y = grafo[j].y;
            grafo[j].y = y;

            grafo[j].nVertice = j;

            ++i;
            --j;
        }
    }
    grafo[p].x = grafo[j].x;
    grafo[p].y = grafo[j].y;
    grafo[p].nVertice = p;

    grafo[j].x = c;
    grafo[j].y = yJ;
    grafo[j].nVertice = j;
    return j;

}

/* quicksort baseado no algoritmo do professor Paulo Feofiloff*/
void quicksort(Ponto grafo[], int p, int r)
{
    int j;
    if(p < r)
    {
        j = separa(grafo, p, r);
        quicksort(grafo, p, j - 1);
        quicksort(grafo, j + 1, r);
    }
}


int recursao(Ponto *grafo, int n, int *jaVisitei, int pontoAnalisado, int *cont)
{
    Conexao *aux;

    /* Testamos se todos os pontos ja foram analisados
    se ja visitamso todos os pontos retornamos TRUE */
    if(*cont == n) return TRUE;

    jaVisitei[pontoAnalisado] = TRUE;
    *cont = *cont + 1;

    /* fazemos chamadas recursivas para analisar os outros pontos */
    for(aux = grafo[pontoAnalisado].conectadoCom; aux != NULL; aux = aux->proxConexao)
    {
        if(jaVisitei[aux->numeroVertice] == FALSE) recursao(grafo, n, jaVisitei, aux->numeroVertice, cont);
    }

    /* Testamos se todos os pontos ja foram analisados
    se ja visitamso todos os pontos retornamos TRUE */
    if(*cont > n - 1) return TRUE;
    else return FALSE;

}



/* cria as arestas do grafo, por meio de uma lista de adjacencia */
void montaGrafo(Ponto *grafo, int N, float d, int queroVerVerts)
{
    int i, j;
    Conexao *aux1, *aux2, *auxil;
    float dist;
    float  intervaloMaior;


    /* ordenamos os vertices, pelo valor da coordena de x */
    quicksort(grafo, 0, N - 1);

    /* lista de adjacencia */
    for(i = 0; i < N; i++)
    {
        /* com os vertices ordenamos, podemos, ao inves de verificar todas as arestas,
        colocar uma distancia maxima que ele pode chegar, no caso é a coordena x do vertice
        somado com a distancia d */
        intervaloMaior = grafo[i].x + d;

        /* verificamos os pontos que podem estar ligados */
        for(j = i + 1; j < N; j++)
        {
            /* quando a coordenaxa x do ponto j passa do valor maximo que a coordenada x
            pode ter damos um break e selecionamos outro ponto */
            if(grafo[j].x > intervaloMaior) break;
            else
            {
                /* se a dist for menor que o d, entao temos uma aresta */
                dist = distanciaPontos(grafo,i,j);
                if(dist <= d)
                {

                    aux1 = mallocSafe(sizeof(Conexao));
                    aux1->distancia = dist;
                    aux1->proxConexao = grafo[i].conectadoCom;
                    aux1->numeroVertice = j;
                    grafo[i].conectadoCom = aux1;

                    /* como a aresta e uma ligaçao de dois vertices, colocamos essa mesma
                    aresta no ponto j */
                    aux2 = mallocSafe(sizeof(Conexao));
                    aux2->distancia = dist;
                    aux2->proxConexao = grafo[j].conectadoCom;
                    aux2->numeroVertice = i;
                    grafo[j].conectadoCom = aux2;
                }
            }


        }



        /* se quisermos ver os vertices */
        if(queroVerVerts == TRUE)
        {
            printf("ponto: %d -- x: %g -- y: %g -- conectadocom: ", grafo[i].nVertice, grafo[i].x, grafo[i].y);
            if(grafo[i].conectadoCom == NULL) printf("NULL\n");
            else
            {
                for(auxil = grafo[i].conectadoCom; auxil != NULL; auxil = auxil->proxConexao )
                {
                    printf("%d, ", auxil->numeroVertice);
                }
            }
            printf("\n");
        }

    }

}













/* essa funçao cria as coordenadas aleatorias dos vertices */
Ponto *geraPontosAleatorios(Ponto *grafo, int seed, int N, int C)
{
    int i;
    float x, y;
    char abreParenteses, virgula, espaco, fechaParantes;


    /* alocamos o vetor dos pontos do grafo */
    grafo = mallocSafe(N * sizeof(Ponto));

    /* geramos o numero aleatorio com a semente */
    srand(seed);


    /* criamos as coordenadas aleatorios dos N pontos */
    if(C == TRUE)
    {

        printf("Digite as coordenadas x e y dos %d pontos da forma (x, y) \n", N);
        for(i = 0; i < N; i++)
        {
            scanf("\n%c%f%c%c%f%c", &abreParenteses, &x, &virgula, &espaco, &y, &fechaParantes);
            grafo[i].x = x;
            grafo[i].y = y;
            grafo[i].nVertice = i;
            grafo[i].conectadoCom = NULL;
        }

    }
    else
    {
        for(i = 0; i < N; i++)
        {
            /* escolhemos a opçao de deixar apenas dois numeros depois da virgula */
            grafo[i].x = rand() %100 / 100.0;
            grafo[i].y = rand() %100 / 100.0;
            grafo[i].nVertice = i;
            grafo[i].conectadoCom = NULL;
        }
    }


    return grafo;
}

/* usamos uma busca binaria para achar um d cada vez mais proximo do menor d que torna o grafo conexo */
float menorDistanciaBusca(Ponto *grafo, int n)
{
    float menorDist;
    float inicioIntervalo = 0, finalIntervalo = sqrt(2), meio;
    float erro = pow(10, -5); /* erro */
    int eConexo;
    int i;

    if(n >= 1000)
    {
        /* verificamos que o menor intervalo pode ser raiz de 2 sobre raiz de n, pois
        podemos provar estatiscamente que e improvavel que a menor distancia seja menor que esse valor
        e tambem pode se provar que e imporvavel que alguma menor distancia exceda raiz( log10(n) * 2) / raiz(n)  */
        inicioIntervalo = 1.4142135 / sqrt(n);
        finalIntervalo = sqrt(log10(n) * 2) / sqrt(n);

    }


    while( (finalIntervalo - inicioIntervalo) > erro )
    {
        meio = (finalIntervalo + inicioIntervalo) /2 ;

        montaGrafo(grafo, n, meio, 0);
        eConexo = conexo(grafo, n);

        for(i = 0; i < n; i++) grafo[i].conectadoCom = NULL; /* resetamos o grafo */

        if(eConexo == TRUE) finalIntervalo = meio;
        else inicioIntervalo = meio;

    }
    menorDist = meio;

    /* restringimos para apenas 4 numeros apos a virgula e somamos 0.0001 pois como resultado da funçao temos
    o maior valor nao conexo, entao somando esse valor temos o menor valor que torna o grafo conexo */
    menorDist =  menorDist / 0.0001;
    menorDist = (int) menorDist;
    menorDist = menorDist /10000 + 0.0001;

    return menorDist;


}


