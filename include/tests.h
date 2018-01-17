#ifndef INCLUDE_TESTS_H_
#define INCLUDE_TESTS_H_



/****************************____TEST1____*************************************/

/**
 * 	@brief	Producer process produces tuples with one element which is integer
 * 	in range <0; 100> every second.
 */
int test1_producerF(int);

/**
 * 	@brief	Consumer1 requests for tuples with one element which is integer <= 50
 * 	(takes half of producer's tuples) every 2 seconds.
 */
int test1_consumer1F(int);

/**
 * 	@brief	Consumer2 requests for tuples with one element which is integer > 50
 * 	(takes half of producer's tuples) every 2 seconds.
 */
int test1_consumer2F(int);

/**
 * 	@brief	Consumer3 requests for tuples with one element which is integer > 95
 * 	every 2 seconds. It tries to get tuple despite of the fact that consumer1 and
 * 	consumer2 cover all produced tuples
 */
int test1_consumer3F(int);


/*
 * ***************************____TEST2____************************************
 *
 * This test uses utilizes test set of 4 tuples (test2_tuples). There are 4
 * client processes which output tuple from tuple test set at the position
 * corresponding to client number. After that client wait for input of tuple
 * which is defined in tuple test set on the position corresponding to client's
 * number + 1. This pattern is repeated in loop. It create chain of dependency
 * between clients. Test shows that cycle is working fine.
 *
 */

/**
 * 	@brief	Client which outputs 0th tuple from given 4-tuple set and
 * 	waits for input of the 1st tuple from. This pattern repeats in the loop.
 */
int test2_client0(int);

/**
 * 	@brief	Client which outputs 1st tuple from given 4-tuple set and
 * 	waits for input of the 2nd tuple from. This pattern repeats in the loop.
 */
int test2_client1(int);

/**
 * 	@brief	Client which outputs 2nd tuple from given 4-tuple set and
 * 	waits for input of the 3rd tuple from. This pattern repeats in the loop.
 */
int test2_client2(int);

/**
 * 	@brief	Client which outputs 3rd tuple from given 4-tuple set and
 * 	waits for input of the 0th tuple from. This pattern repeats in the loop.
 */
int test2_client3(int);


/*
 * ***************************____TEST3____************************************
 *
 * This test uses utilizes test set of 4 tuples (test2_tuples). There are 4
 * client processes which output tuple from tuple test set on the position
 * corresponding to client number. After that client wait for input of tuple
 * which is defined in tuple test set on the position corresponding to client's
 * number + 1. This pattern is repeated in loop. After specified number of loops
 * the last client exits and whole chain-tuple-passing process between clients is
 * broken. All remaining clients hang while waiting for the exited client to
 * push its tuple.
 *
 */

/**
 * 	@brief	Client which outputs 0th tuple from given 4-tuple set (test3_tuples) and
 * 	waits for input of the 1st tuple from test3_tuples. This pattern repeats in the loop.
 */
int test3_client0(int);

/**
 * 	@brief	Client which outputs 1st tuple from given 4-tuple set and
 * 	waits for input of the 2nd tuple from test3_tuples. This pattern repeats in the loop.
 */
int test3_client1(int);

/**
 * 	@brief	Client which outputs 2nd tuple from given 4-tuple set and
 * 	waits for input of the 3rd tuple from test3_tuples. This pattern repeats in the loop.
 */
int test3_client2(int);

/**
 * 	@brief	Client which outputs 3rd tuple from given 4-tuple set and
 * 	waits for input of the 0th tuple from test3_tuples. This pattern repeats few times
 * 	and then client3 stops. In result all other clients are blocked for timeout which they have.
 */
int test3_client3(int);



/*
 * ***************************____TEST4____************************************
 *
 * This test uses utilizes test set of 10 tuples (test4_tuples). Producer process outputs
 * tuples from this test set sequentially, every 1 second and if reaches end of test4_tuples
 * if begins from the 0th tuple and repeats this process in infinite loop - cyclically send
 * entire test vector of tuples. There are 10 consumer processes which wait only one tuple from
 * tests vector (test set) - tuple which is at position equal to consumer's number. Each consumer
 * has timeout set to test4_tuples.size() + 1 seconds. In result each of the customer process
 * should be provided with requested tuple on time.
 *
 */

// Test vector for test number 4.
extern std::vector<Tuple> test4_tuples;

/**
 * 	@brief	Sends each tuple from test vector (test4_tuples) sequentially to tuple space
 * 	every 1 sec. and does it in the cyclic way (if reaches vector end it starts from the beginning).
 */
int test4_producer(int);

/**
 * 	@brief	Waits for tuple from test4_tuples which has position in the vector equal to consumer number.
 * 	Timeout is set to test4_tuples.size() + 1 seconds so all consumers should be serviced in one
 * 	producer's vector cycle.
 *
 * 	@param	tupleNumber is position of tuple in test vector which tuple is requested by this function
 */
int test4_consumer(int tupleNumber);



/*
 * ***************************____TEST5____************************************
 *
 * This test uses utilizes test set of 10 tuples (test5_tuples). Producer process outputs
 * tuples from this test set sequentially, every 1 second and if reaches end of test5_tuples
 * if begins from the 0th tuple and repeats this process in infinite loop - cyclically send
 * entire test vector of tuples. There are 10 consumer processes which wait only one tuple from
 * tests vector (test set) - each consumer waits for another tuple. Five consumers are patient
 * and have timeout set to greater than one cycle of producer and will always be serviced.
 * Another five consumers are impatient and have timeout set to less than producer cycle period
 * and will not be serviced in some loops. Impatient consumers will resign from requested tuple
 * which should be seen on the screen.
 *
 */

// Test vector for test number 5.
extern std::vector<Tuple> test5_tuples;

/**
 * 	@brief	Sends each tuple from test vector (test5_tuples) sequentially to tuple space
 * 	every 1 sec. and does it in the cyclic way (if reaches vector end it starts from the beginning).
 */
int test5_producer(int);

/**
 * 	@brief	Waits for tuple from test4_tuples which has position in the test vector equal to given argument.
 *  Timeout for patient consumers is set to test4_tuples.size() + 1 seconds so all patient
 * 	consumers should be serviced in one producer's vector cycle.
 *
 * 	@param	tupleNumber is position of tuple in test vector which tuple is requested by this function
 */
int test5_patient_consumer(int tupleNumber);

/**
 * 	@brief	Waits for tuple from test4_tuples which has position in the test vector equal to given argument.
 *  Timeout for impatient consumers is set to less than test4_tuples.size() seconds so all impatient
 * 	consumers should be NOT always serviced in one producer's vector cycle and should resign after timeout.
 *
 * 	@param	tupleNumber is position of tuple in test vector which tuple is requested by this function
 */
int test5_impatient_consumer(int tupleNumber);

#endif /* INCLUDE_TESTS_H_ */
