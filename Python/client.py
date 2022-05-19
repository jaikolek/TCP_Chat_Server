from concurrent.futures import thread
import threading
import socket

host = '127.0.0.1' #ip for localhost
port = 12000 #can use random port just dont use an established one

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
result = client.connect_ex((host, port))

#check if server is online or not
print("Checkint server connection...")
if result == 0:
    nickname = input("Enter your nickname: ")
else:
    print("Server offline...")
    client.close()
    quit()

#recieve message from server
def recieve():
    while True:
        try:
            message = client.recv(1024).decode('ascii')
            if message == 'nick': #if server sent 'nick' respond with sending client's nickname
                client.send(nickname.encode('ascii'))
            else: #if not print the message
                print(message) 
        except:
            print("An Error have occured")
            client.close()
            break

#write message and send to server
def write():
    while True:
        message = f'{nickname}: {input("")}'
        client.send(message.encode('ascii'))

recieve_thread = threading.Thread(target = recieve)
recieve_thread.start()

write_thread = threading.Thread(target = write)
write_thread.start()