build: client

client:
	gcc helpers.c parson.c requests.c buffer.c client.c -o $@ -lssl -lcrypto

PHONY: clean
clean:
	rm -f client
		