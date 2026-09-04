ITA:
Questo sketch di Arduino permette eseguire i test statici della thrust e della torque. 

Il sistema è dotato di un display LCD per visualizzare rapidamente i valori misurati e di pulsanti per attivare la modalità "setup manuale" in cui è possibile modificare (aumentare o diminuire di 5) i valori del fattore di calibrazione per torque e thrust. Il pulsante centrale, se premuto una volta, attiva la regolazione manuale del fattore di calibrazione per la misura di thrust, se premuto un'altra volta attiva la regolazione manuale del fattore di calibrazione per la misura di torque, se premuto ancora una volta disattiva la modalità di "setup manuale". Una volta attivata una delle due regolazioni, premendo il pulsante a destra di aumenterà di 5 il relativo fattore di calibrazione mentre premendo il pulsante a sinistra lo si diminuirà di 5.

In questi test è previsto l'utilizzo del software Coolterm che permette di visualizzare i messaggi contenenti le istruzioni da seguire per eseguire i test e i valori di thrust campionati. Questo software inoltre permette di "registrare" tutto ciò che viene stampato/visualizzato a schermo in un file di testo che è possibile nominare nel modo che più si preferisce. 

Per eseguire Coolterm non è necessario scaricare/installare nulla, basta semplicemente scaricare il file .zip, presente in questa cartella del drive, estrarlo e lanciare il software (attenzione a non rimuovere però il file .exe dalla cartella in cui è contenuto, altrimenti non funziona).

Per eseguire il test bisogna assicurarsi che i collegamenti tra le schede elettroniche per cella di carico (HX711) e la scheda Arduino siano corretti (fare riferimento allo schema presente in questa cartella del drive), bisogna caricare lo sketch "sketch_static_test.ino" su una scheda Arduino (senza Coolterm in esecuzione) e infine bisogna aprire Coolterm. Se lo sketch è già stato caricato sulla scheda si può saltare il secondo passaggio e passare direttamente all'apertura di Coolterm.

Una volta aperto Coolterm bisogna scegliere (in basso a sinistra) la porta seriale alla quale è collegata la scheda Arduino e successivamente bisogna premere su "Connect" (in alto in centro). Una volta connessi alla scheda bisogna attendere la comparsa dei messaggi che vi guideranno nello svolgimento dei test. Una volta terminati i test premere su "Disconnect" per disconnettersi dalla scheda e chiudere tutto.

Per registrare i dati bisogna andare su "Connection" (presente nel menù in alto) -> Capture to text/Binary file -> Start. A quel punto vi verrà chiesto il nome da dare al file di testo che verrà generato e dove salvarlo. Quando non si vuole più registrare i dati basta seguire il percorso di prima e premere "Stop" (Se si vuole solo mettere in pausa la raccolta dei dati premere "Pause").

I messaggi stampati a schermo vi aiuteranno nello svolgimento dei test, l'unico aspetto caratteristico che merita una spiegazione più dettagliata è il seguente: Ogni volta che si attiva una sequenza di campionamento verrà settato il nuovo zero_factor sia per la misura di thrust che per la misura di torque, così facendo tra un test ed il successivo non verranno considerate le eventuali micro-deformazioni della cella che potrebbero non riportarla alla condizione di partenza e potrebbero incidere sulle misure. In questo modo ad ogni prova si va a considerare la sola deformazione dovuta al test che viene effettuato.

ENG:
This Arduino sketch allows for static testing of thrust and torque.

The system is equipped with an LCD display to quickly visualize the measured values and buttons to activate the "manual setup" mode, where it is possible to adjust (increase or decrease by 5) the calibration factor values for torque and thrust. The center button, when pressed once, enables manual calibration adjustment for thrust measurement; when pressed again, it enables manual calibration adjustment for torque measurement; pressing it once more deactivates the "manual setup" mode. Once one of the two adjustments is activated, pressing the right button increases the corresponding calibration factor by 5, while pressing the left button decreases it by 5.

For these tests, the CoolTerm software is used, allowing users to view messages with instructions for performing the tests and displaying sampled thrust values. Additionally, this software enables the recording of everything displayed on the screen into a text file, which can be named as desired.

To run CoolTerm, no installation is required. Simply download the .zip file from this Drive folder, extract it, and launch the software (be careful not to remove the .exe file from its folder, as it will not function otherwise).

To perform the test, ensure that the connections between the load cell electronic boards (HX711) and the Arduino board are correct (refer to the schematic available in this Drive folder). Then, upload the "sketch_static_test.ino" sketch to an Arduino board (without CoolTerm running), and finally, open CoolTerm. If the sketch has already been uploaded to the board, you can skip the second step and proceed directly to opening CoolTerm.

Once CoolTerm is open, select (bottom left) the serial port to which the Arduino board is connected, then press "Connect" (top center). Once connected to the board, wait for the messages that will guide you through the tests. After completing the tests, press "Disconnect" to disconnect from the board and close everything.

To record data, go to "Connection" (in the top menu) -> "Capture to text/Binary file" -> "Start." At this point, you will be prompted to enter a name for the text file and choose where to save it. When you no longer wish to record data, follow the same path and press "Stop" (if you only want to pause data collection, press "Pause").

The messages displayed on the screen will guide you through the tests. However, one important aspect that requires a more detailed explanation is the following: Every time a sampling sequence is activated, a new zero_factor is set for both thrust and torque measurements. This ensures that any micro-deformations of the load cell between tests, which might prevent it from returning to its initial state and affect measurements, are not taken into account. This way, each test considers only the deformation caused by the specific test being performed.