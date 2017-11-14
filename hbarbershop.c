#include<pthread.h> //biblioteca de threads
#include<semaphore.h> //biblioteca de semaforos
#include<stdlib.h>
#include<stdio.h>

#define MAX 20 //Capacidade máxima da barbearia
#define QUANT_CLIENTE 10 //quantidade de clientes "para ser atendido aquele dia"

int clientes = 0; //inicia sem clientes
int nro_cadeira = 3;
int nro_assento_sofa = 4;
int nro_barbeiros = 3;

pthread_t nCliente[QUANT_CLIENTE];

pthread_t barb[3]; //três barbeiros

/*declara os semáforos utilizados*/
/*mutex protege clientes, acompanha o número de clientes na barbearia*/
sem_t mutex, cadeira, barbeiro, cliente, dinheiro, recebimento, registradora; 

//estrutura da fila
typedef struct{
	sem_t leader; //lider
	sem_t follower; //seguidor
} Fifo; //fila

/*declara duas filas, uma para a sala e outra para o sofá*/
Fifo *salaDeEspera, *sofa;  

//cria fila
Fifo* cria_fila(int n){ 
	Fifo* F=(Fifo*)malloc(sizeof(Fifo)); //aloca memória para a fila F

	sem_init(&(F->leader), 0,0); 
	sem_init(&(F->follower),0,n);
	return (F); //retorna a fila criada
}


void wait_Fifo(Fifo* F, int n){ 
	sem_wait(&(F->follower));
	sem_post(&(F->leader));
}

void signal_Fifo(Fifo* F) 
{ 
	sem_wait(&(F->leader)); 
	sem_post(&(F->follower)); 
} 

/*prototipo das funções*/

void sai_da_barbearia(){
}
void esperando_na_sala_de_espera(){
}
void entra_na_barbearia(){
}
void espera_sofa(){
}
void senta_no_sofa(){
}
void standing_room_signal(){
}
void senta_na_cadeira_do_barbeiro(){
}
void corta_cabelo(){
}
void paga(){
}
//metodo da barbearia
void* barbearia(void *arg){		
	int n=*(int*) arg;
	
	
	//while(1)
	//{
		sem_wait(&mutex); 

		/* Verifica se a barbearia está cheia, se ela estiver com 
		sua capacidade máxima, o cliente que entrou sai da barbearia*/
		
		if(clientes>=20) 
		{
			sem_post(&mutex); 
			printf("Saindo da barbearia %d..\n",n); 
			sai_da_barbearia(); //metodo criado		
		}
		
		/* A barbearia não está lotada, então o cliente entra, 
		e atualiza o numero de clientes*/
		clientes+=1;  
		sem_post(&mutex); 

		wait_Fifo(salaDeEspera,n);
		esperando_na_sala_de_espera(); 

		printf("\n Cliente %d entrando na sala de espera\n",n);
		entra_na_barbearia(); 
		wait_Fifo(sofa,n);
		espera_sofa(); 
		//sleep(1);

		printf("\n Cliente %d sentando no sofá\n",n);
		senta_no_sofa(); 
		//sleep(2); 

		signal_Fifo(salaDeEspera);
		standing_room_signal(); 

		sem_wait(&cadeira); 
		printf("\n Cliente %d senta na cadeira do barbeiro\n",n);
		sleep(3); 
		signal_Fifo(sofa); 
 
		sem_post(&cliente); 

		sem_wait(&barbeiro); 
		printf("\n Cliente %d corta cabelo\n",n); 
		corta_cabelo();
		sleep(2); 

		printf("\n Cliente %d paga\n",n);
		paga();
		//sleep(1);

		sem_post(&dinheiro); 
		sem_wait(&recebimento); 
		sem_wait(&mutex); 
		clientes-=1; //decrementa o cliente que pagou
		sem_post(&mutex); 
		printf("\n Cliente %d sai da barbearia\n",n); 
		//sleep(2); 
		sai_da_barbearia(); 
		
	//} 
}
 
 //metodo cortar cabelo
void* cortando(void* arg) 
{ 
	int n=*(int *)arg; 
	while(1) 
        { 
		sem_wait(&cliente); 
		sem_post(&barbeiro); 
		printf("\tBarbeiro %d cortando cabelo\n",n); 
		//sleep(3); 
		sem_wait(&dinheiro); 
		printf("\tBarbeiro %d aceitando pagamento\n",n); 
		//sleep(4); 
		sem_post(&recebimento); //recebe o dinheiro do cliente
		sem_post(&registradora); //leva o dinheiro para a caixa registradora
		printf("\tBarbeiro %d leva dinheiro na registradora\n", n);
		sem_post(&cadeira); 
      } 
} 

main() //função main
{ 
	int i,nCliente_id[QUANT_CLIENTE]; 
	sem_init(&mutex,0,1); 
	sem_init(&cadeira,0,3); 
	sem_init(&barbeiro,0,0); 
	sem_init(&cliente,0,0); 
	sem_init(&dinheiro,0,0); 
	sem_init(&recebimento,0,0); 
	sem_init(&registradora,0,0);

	salaDeEspera=cria_fila(16); //cria uma sala de espera com 16 posições
	sofa=cria_fila(nro_assento_sofa); //cria o sofá com 4 posições

	for(i=0;i<QUANT_CLIENTE;i++){ 
		nCliente_id[i]=i; 
		pthread_create(&nCliente[i],0,barbearia,&nCliente_id[i]); 
      	} 
	for(i=0;i<3;i++) 
		pthread_create(&barb[i],0,cortando,&nCliente_id[i]); 
	while(1); 
} 
















