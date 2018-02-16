all: master producer consumer

master: master.c
	gcc -lrt -o master master.c
	
producer: producer.c
	gcc -o producer producer.c
	
consumer: consumer.c
	gcc -o consumer consumer.c
	
clean:
	rm master producer consumer