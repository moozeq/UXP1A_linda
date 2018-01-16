#ifndef INCLUDE_TESTS_H_
#define INCLUDE_TESTS_H_

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



int test2_client1(void);

int test2_client2(void);

int test2_client3(void);

int test2_client4(void);

#endif /* INCLUDE_TESTS_H_ */
