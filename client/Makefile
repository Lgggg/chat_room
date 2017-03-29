../bin/input_client : input_client.c show_client.c shm_and_sem.c shm_and_sem.h client_msg.h
	gcc -g -o $@ input_client.c shm_and_sem.c
	gcc -g -o ../bin/show_client show_client.c shm_and_sem.c

clean:
	rm -rf ../bin/input_client ../bin/show_client
