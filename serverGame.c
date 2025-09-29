#include "serverGame.h"
#include <pthread.h>

// -----
tPlayer getNextPlayer(tPlayer currentPlayer)
{

	tPlayer next;

	if (currentPlayer == player1)
		next = player2;
	else
		next = player1;

	return next;
}

void initDeck(tDeck *deck)
{

	deck->numCards = DECK_SIZE;

	for (int i = 0; i < DECK_SIZE; i++)
	{
		deck->cards[i] = i;
	}
}

void clearDeck(tDeck *deck)
{

	// Set number of cards
	deck->numCards = 0;

	for (int i = 0; i < DECK_SIZE; i++)
	{
		deck->cards[i] = UNSET_CARD;
	}
}

void printSession(tSession *session)
{

	printf("\n ------ Session state ------\n");

	// Player 1
	printf("%s [bet:%d; %d chips] Deck:", session->player1Name, session->player1Bet, session->player1Stack);
	printDeck(&(session->player1Deck));

	// Player 2
	printf("%s [bet:%d; %d chips] Deck:", session->player2Name, session->player2Bet, session->player2Stack);
	printDeck(&(session->player2Deck));

	// Current game deck
	if (DEBUG_PRINT_GAMEDECK)
	{
		printf("Game deck: ");
		printDeck(&(session->gameDeck));
	}
}

// -----
void initSession(tSession *session)
{

	clearDeck(&(session->player1Deck));
	session->player1Bet = 0;
	session->player1Stack = INITIAL_STACK;

	clearDeck(&(session->player2Deck));
	session->player2Bet = 0;
	session->player2Stack = INITIAL_STACK;

	initDeck(&(session->gameDeck));
}

// -----
unsigned int calculatePoints(tDeck *deck)
{

	unsigned int points;

	// Init...
	points = 0;

	for (int i = 0; i < deck->numCards; i++)
	{

		if (deck->cards[i] % SUIT_SIZE < 9)
			points += (deck->cards[i] % SUIT_SIZE) + 1;
		else
			points += FIGURE_VALUE;
	}

	return points;
}

// -----
unsigned int getRandomCard(tDeck *deck)
{

	unsigned int card, cardIndex, i;

	// Get a random card
	cardIndex = rand() % deck->numCards;
	card = deck->cards[cardIndex];

	// Remove the gap
	for (i = cardIndex; i < deck->numCards - 1; i++)
		deck->cards[i] = deck->cards[i + 1];

	// Update the number of cards in the deck
	deck->numCards--;
	deck->cards[deck->numCards] = UNSET_CARD;

	return card;
}

/**
 * Accept connection
 */
int acceptConnection(int socketServer)
{

	int clientSocket;
	struct sockaddr_in clientAddress;
	unsigned int clientAddressLength;

	// Get length of client address
	clientAddressLength = sizeof(clientAddress);

	// Accept
	if ((clientSocket = accept(socketServer, (struct sockaddr *)&clientAddress, &clientAddressLength)) < 0)
		showError("Error while accepting connection");

	printf("Connection established with client: %s\n", inet_ntoa(clientAddress.sin_addr));

	return clientSocket;
}

#define MAX_CONNECTIONS 5
/**
 * Create, bind and listen
 */
int createBindListenSocket(unsigned short port)
{

	int socketId;
	struct sockaddr_in echoServAddr;

	if ((socketId = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		showError("Error while creating a socket");

	// Set server address
	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family = AF_INET; //El servidor usara Ipv4	
	echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY significa que escuchara en cualquier red disponible
	echoServAddr.sin_port = htons(port); //Indica el puerto que escuchara el servidor

	// Bind
	if (bind(socketId, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) < 0)
		showError("Error while binding");

	if (listen(socketId, MAX_CONNECTIONS) < 0)
		showError("Error while listening");

	return socketId;
}
void process_game(int socketPlayer1, int socketPlayer2)
{
	tSession session;

	strcpy(session.player1Name, "Name1");

	strcpy(session.player2Name, "Name2");
	clearDeck(&session.gameDeck);
	initDeck(&session.gameDeck);

	while (1)
	{ // jugar rondas

		// iniciar ronda
		// limpiar y iniciar mazos aleatoriamente con 2 cartas
		clearDeck(&session.player1Deck);
		clearDeck(&session.player2Deck);
		for (int i = 0; i < 2; i++)
		{
			getRandomCard(&session.player1Deck);
			getRandomCard(&session.player2Deck);
		}

		int jugador1_sigue = 1;
		int jugador2_sigue = 1;
		tPlayer turno = player1;
		while (1)
		{ // decisiones jugadores

			// recibir datos player 1
			if (jugador1_sigue)
			{
				int bytes_recibidos = recv(socketPlayer1, &session.player1Bet, sizeof(int), 0);
				int opcion_elegida1;

				if (bytes_recibidos < 0)
					showError("ERROR while reading from socket 1");

				bytes_recibidos = recv(socketPlayer1, &opcion_elegida1, sizeof(int), 0);

				if (bytes_recibidos < 0)
					showError("ERROR while reading from socket 1");

				if (opcion_elegida1 == 1)
				{
					getRandomCard(&session.player1Deck);
				}
				else
					jugador1_sigue = 0;

				if (calculatePoints(&session.player1Deck) >= 21)
					jugador1_sigue = 0;
			}

			// recibir datos player 2

			if (jugador2_sigue)
			{
				int bytes_recibidos = recv(socketPlayer2, &session.player2Bet, sizeof(int), 0);
				int opcion_elegida2;

				if (bytes_recibidos < 0)
					showError("ERROR while reading from socket 2");

				bytes_recibidos = recv(socketPlayer2, &opcion_elegida2, sizeof(int), 0);

				if (bytes_recibidos < 0)
					showError("ERROR while reading from socket 2");
			}

			if (opcion_elegida2 == 1)
			{
				getRandomCard(&session.player2Deck);
			}
			else
				jugador2_sigue = 0;

			if (calculatePoints(&session.player2Deck) >= 21)
				jugador2_sigue = 0;
		}
	}
}

		int bytes_recibidos = recv(socketPlayer1, &fichas_apostadas1, sizeof(int), 0);

void receiveData(int socketPlayer, int& data){
		int received_bytes = recv(socketPlayer, data, sizeof(int), 0);
		if (received_bytes < 0) showError("ERROR while reading from socket 1");
}
void addCardtoDeck(tDeck& deck,int card){
	if(deck.numCards >= DECK_SIZE) showError("ERROR, DECK IS FULL");
	deck.numCards++;
	deck.cards[deck.numCards - 1] = card;
}
int main(int argc, char *argv[])
{

	int socketfd;					   /** Socket descriptor */
	struct sockaddr_in serverAddress;  /** Server address structure */
	unsigned int port;				   /** Listening port */
	struct sockaddr_in player1Address; /** Client address structure for player 1 */
	struct sockaddr_in player2Address; /** Client address structure for player 2 */
	int socketPlayer1;				   /** Socket descriptor for player 1 */
	int socketPlayer2;				   /** Socket descriptor for player 2 */
	unsignºed int clientLength;		   /** Length of client structure */
	tThreadArgs *threadArgs;		   /** Thread parameters */
	pthread_t threadID;				   /** Thread ID */
	tSession session;				   /** Game's session */
	tDeck deckPlayer1;
	tDeck deckPlayer2;
	int card;
	// Seed
	srand(time(0));

	// Check arguments
	if (argc != 2)
	{
		fprintf(stderr, "ERROR wrong number of arguments\n");
		fprintf(stderr, "Usage:\n$>%s port\n", argv[0]);
		exit(1);
	}

	// Get the port server
	port = atoi(argv[1]);

	// Create a socket (also bind and listen)
	socketfd = createBindListenSocket(port);

	// establece conexion con 2 jugadores
		// espera hasta que el jugador1 establezca conexion
		socketPlayer1 = acceptConnection(socketfd);

		if (socketPlayer1 < 0)
			showError("ERROR while accepting player 1");

		// espera hasta que el jugador2 establezca conexion
		socketPlayer2 = acceptConnection(socketfd);

		if (socketPlayer2 < 0)
			showError("ERROR while accepting player 2");

	// Infinite loop...
	int continuar = 1;

	initSession(&session);
	while (continuar)
	{
		// recibir apuesta jugador 1
		int fichas_apostadas1;
		receiveData(socketPlayer1,fichas_apostadas1);

		// recibir apuesta jugador 2
		int fichas_apostadas2;
		receiveData(socketPlayer2,fichas_apostadas2);


		//Tras haber recivido las apuestas, dar a ambos jugadores sus 2 cartas iniciales
		for(int i = 0; i < 2; i++){
			card = getRandomCard(&session.gameDeck);
			addCardtoDeck(session.player1Deck,card);
		}
		for(int i = 0; i < 2; i++){
			card = getRandomCard(&session.gameDeck);
			addCardtoDeck(session.player2Deck,card);
		}
		
		int opcion_elegida1;
		bytes_recibidos = recv(socketPlayer1, &opcion_elegida1, sizeof(int), 0);

		int opcion_elegida2;
		bytes_recibidos = recv(socketPlayer2, &opcion_elegida2, sizeof(int), 0);

		if (bytes_recibidos < 0)
			showError("ERROR while reading from socket 2");
			
		if (bytes_recibidos < 0)
			showError("ERROR while reading from socket 1");
		/*
		para depurar mostrar por pantalla
		*/
		printf("%d\n", fichas_apostadas1);
		printf("%d\n", opcion_elegida1);
		printf("%d\n", fichas_apostadas2);
		printf("%d\n", opcion_elegida2);
		/*
		// Allocate memory
		if ((threadArgs = (struct ThreadArgs *) malloc(sizeof(struct ThreadArgs))) == NULL)
			showError("Error while allocating memory");


		// Set socket to the thread's parameter structure
		threadArgs->clntSock = clientSocket;


		// Create a new thread
		if (pthread_create(&threadID, NULL, threadProcessing, (void *) threadArgs) != 0)
			showError("pthread_create() failed");
			*/

		while (1)
			;
		continuar = 0;
	}
}
