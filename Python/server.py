import threading
import socket

host = '127.0.0.1' #ip for localhost
port = 12000 #can use random port just dont use an established one

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM) #make server
server.bind((host, port)) #bind server to ip
server.listen() #server listening for req

#store clients and their nicknames
clients = []
nicknames = []

limitUser = 2 #limit user that able to connect

#broadcast message from client to all client
def broadcast(message):
    for client in clients:
        client.send(message)

#handle client if messaging or leaving
def handle(client):
    while True:
        try:
            #recieving message and broadcast it
            message = client.recv(1024)
            broadcast(message)
        except:
            #removing client, closing connection
            index = clients.index(client)
            clients.remove(client)
            client.close()
            nickname = nicknames[index]
            broadcast(f'{nickname} left room chat...'.encode('ascii'))
            print(f'{nickname} disconnected')
            nicknames.remove(nickname)
            break

#server when recieving / a client connected
def recieve():
    while len(clients) < limitUser: #if limit exceed, stop accepting client
        client, address = server.accept()
        print(f"Connected with {str(address)}")
        
        #getting client nickname
        client.send('nick'.encode('ascii'))
        nickname = client.recv(1024).decode('ascii')
        
        #storing it on the array
        nicknames.append(nickname)
        clients.append(client)
        
        print(f'Client nickname is {nickname}...')
        broadcast(f'{nickname} joined room chat...'.encode('ascii'))
        client.send('Connected to server...'.encode('ascii'))

        thread = threading.Thread(target = handle, args = (client,))
        thread.start()
    while len(clients) > limitUser:
        client, address = server.accept()
        print(f"Connected with {str(address)}")

print("Server is listening...")
recieve() #call recieve method