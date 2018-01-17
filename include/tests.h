#ifndef INCLUDE_TESTS_H_
#define INCLUDE_TESTS_H_



/****************************____TEST1____*************************************/

/**
 * 	@brief	Producer process produces tuples with one element which is integer
 * 	in range <0; 100> every second.
 */
int test1_producerF(void);

/**
 * 	@brief	Consumer1 requests for tuples with one element which is integer <= 50
 * 	(takes half of producer's tuples) every 2 seconds.
 */
int test1_consumer1F(void);

/**
 * 	@brief	Consumer2 requests for tuples with one element which is integer > 50
 * 	(takes half of producer's tuples) every 2 seconds.
 */
int test1_consumer2F(void);

/**
 * 	@brief	Consumer3 requests for tuples with one element which is integer > 95
 * 	every 2 seconds. It tries to get tuple despite of the fact that consumer1 and
 * 	consumer2 cover all produced tuples
 */
int test1_consumer3F(void);


/*
 * ***************************____TEST2____************************************
 *
 * This test uses utilizes test set of 4 tuples (test2_tuples). There are 4
 * client processes which output tuple from tuple test set on the position
 * corresponding to client number. After that client wait for input of tuple
 * which is defined in tuple test set on the position corresponding to client's
 * number + 1. This pattern is repeated in loop.
 *
 */

/**
 * 	@brief	Client which outputs 0th tuple from given 4-tuple set and
 * 	waits for input of the 1st tuple from. This pattern repeats in the loop.
 */
int test2_client0(void);

/**
 * 	@brief	Client which outputs 1st tuple from given 4-tuple set and
 * 	waits for input of the 2nd tuple from. This pattern repeats in the loop.
 */
int test2_client1(void);

/**
 * 	@brief	Client which outputs 2nd tuple from given 4-tuple set and
 * 	waits for input of the 3rd tuple from. This pattern repeats in the loop.
 */
int test2_client2(void);

/**
 * 	@brief	Client which outputs 3rd tuple from given 4-tuple set and
 * 	waits for input of the 0th tuple from. This pattern repeats in the loop.
 */
int test2_client3(void);


/*
 * ***************************____TEST2____************************************
 *
 * This test uses utilizes test set of 4 tuples (test2_tuples). There are 4
 * client processes which output tuple from tuple test set on the position
 * corresponding to client number. After that client wait for input of tuple
 * which is defined in tuple test set on the position corresponding to client's
 * number + 1. This pattern is repeated in loop.
 *
 */

/**
 * 	@brief	Client which outputs 0th tuple from given 4-tuple set (test3_tuples) and
 * 	waits for input of the 1st tuple from test3_tuples. This pattern repeats in the loop.
 */
int test3_client0(void);

/**
 * 	@brief	Client which outputs 1st tuple from given 4-tuple set and
 * 	waits for input of the 2nd tuple from test3_tuples. This pattern repeats in the loop.
 */
int test3_client1(void);

/**
 * 	@brief	Client which outputs 2nd tuple from given 4-tuple set and
 * 	waits for input of the 3rd tuple from test3_tuples. This pattern repeats in the loop.
 */
int test3_client2(void);

/**
 * 	@brief	Client which outputs 3rd tuple from given 4-tuple set and
 * 	waits for input of the 0th tuple from test3_tuples. This pattern repeats few times
 * 	and then client3 stops. In result all other clients are blocked for timeout which they have.
 */
int test3_client3(void);
#endif /* INCLUDE_TESTS_H_ */
