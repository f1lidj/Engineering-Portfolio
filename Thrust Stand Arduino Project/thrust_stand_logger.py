'''
Requisiti:
- sistema operativo Windows 64-bit
- una distribuzione di Python3 installata sul sistema (scaricabile da https://www.python.org/downloads/windows/)
- modulo "pyserial" installato sulla distribuzione di Python in uso

Installazione di pyserial:
    aprire una finestra di terminale (ricerca "cmd" dalla ricerca Windows) ed eseguire il comando:
    pip3 install pyserial

UTILIZZO DEL PROGRAMMA:
    Il programma va eseguito a seguito del collegamento fisico della seriale di Arduino col PC.
    Il programma riconoscerà automaticamente la porta e creerà il collegamento seriale.
    Una volta stabilito il collegamento, Arduino si riavvierà automaticamente e verranno printati in tempo reale i messaggi di Arduino sulla console.
    In ogni momento è possibile premere tasti sulla tastiera per comunicare con Arduino in tempo reale.

    Per eseguire un test statico sullo stand, premere "1" come suggerito dal menù principale.
    Partirà un timer di 3 secondi, dopodiché inizierà l'acquisizione e i valori saranno printati nella console in tempo reale e registrati dal programma.
    Per terminare l'acquisizione e tornare al menù principale di Arduino, premere la barra spaziatrice.

    Per chiudere la comunicazione seriale e terminare il programma in qualunque momento, premere il tasto E.
    Alla chiusura del programma, verrà automaticamente creato un file di log a partire dalla registrazione della sessione.

IMPORTANTE:
    Non eseguire più di un test durante una sessione, poiché il file di log è unico per ogni sessione.
    Se, una volta tornato al menù, viene iniziata una seconda acquisizione, questa verrà salvata insieme alla prima (che comunque non è grave).
    Per iniziare un altro test, premere E per uscire dal programma e rieseguire nuovamente lo script.
'''


import serial, re, threading, msvcrt, csv, os
import serial.tools.list_ports as serial_ports

LOG_PATH = 'Logs'                   # folder where logs are stored
LOG_NAME = 'thrust_stand_log'       # name of log files

# Function for threading (listening and printing Arduino output)
buff = ''
def serial_listener(running):
    global buff
    while running.is_set():
        if arduino.in_waiting:
            r = arduino.read().decode()
            if r == '\x00' or r == '\r':
                continue
            print(r, end='', flush=True)
            buff += r

'''
for comport in serial_ports.comports():
    print("Porta:", comport.device)
    print("  Serial:", comport.serial_number)
    print("  Descrizione:", comport.description)         
'''
''' 
# Default values for our Arduino
SERIAL_NUMBER = '75630313536351918231'
'''
BAUD = 9600
  

COM = None
for comport in serial_ports.comports():
    if "USB-SERIAL CH340" in comport.description:
        COM = comport.device
        break

if COM is None:
    raise RuntimeError("Arduino non trovato!")

''' Part of code changed due to the new (fake) Arduino Nano which doesn't show the serial number
# Find the correct serial port for Arduino
COM = None
for comport in serial_ports.comports():
    if comport.serial_number == SERIAL_NUMBER:
        COM = comport.name
        break

if not(COM):
    raise RuntimeError('Arduino not found!')
'''
# Connect to Arduino
arduino = serial.Serial(port=COM, baudrate=BAUD)

# Listen and print Arduino output in real-time
running = threading.Event()
running.set()
listen_thread = threading.Thread(target=serial_listener, args=(running,))
listen_thread.start()

# Sends the keyboard inputs to Arduino in real-time (while printing Arduino output)
while True:
    x = msvcrt.getch()
    if x.decode().lower() == 'e':
        break
    else:
        arduino.write(x)

# Close the listener thread and the serial
running.clear()
listen_thread.join()
arduino.close()

# Check if log folder already exists, otherwise create it
if not(os.path.exists(LOG_PATH)):
    os.makedirs(LOG_PATH)

# Retrieve the last log number present in the folder to create the next one
log_list = [0]
file_list = os.listdir(LOG_PATH)
for file in file_list:
    m = re.match(LOG_NAME + r'_(\d{3}).csv', file)
    if m:
        log_list.append(int(m.group(1)))
last_log_num = max(log_list)

# Construct the filename (and path)
filename = LOG_PATH + '\\' + LOG_NAME + '_' + '{:0>3}'.format(last_log_num+1) + '.csv'

# Create the log file reading from buffer
data = re.findall(r'(\d+):\s*(-*[\d\.]+)\s+(-*[\d\.]+)', buff)
with open(filename, 'w', newline='') as csvfile:
    writer = csv.writer(csvfile)
    writer.writerow(('Time [ms]', 'Thrust [N]', 'Torque [Nm]'))
    writer.writerows(data)

input('\nProgram terminated! Press Enter to exit...')