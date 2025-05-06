- Γενική: 
    1. Τα αρχεία έχουν χωριστεί στα απαραίτητα directories, όπως ζητήθηκε.
    2. Πιστεύω πως έχω καλύψει όλα τα ζητούμενα τις εργασίας.
    3. Ονοματεπώνυμο: Δημήτριος Χρυσός
    4. Αριθμός Μητρώου: 1115202100275

- Compilation και εκτέλεση:
    - Έχει φτιαχτεί Makefile που κάνει αυτόματα τη διαδικασία του compilation και της διαγραφής 
    παλιών bin files.
    - Για το compilation χρειάζεται απλά η εντολή «make» σε κάποιο terminal.
    - Για το άνοιγμα του server χρειάζεται σε ένα terminal η εντολή: 
    «./bin/jobExecutorServer [portnum] [bufferSize] [threadPoolSize]»
    - Αν θέλουμε να τρέξουμε τα test από το directory tests, πρέπει να ρυθμίσουμε το 
    αρχείο «server_host.sh», με το «SERVER_HOST» και το «PORT» που θέλουμε.
    - Τα test τρέχουν, σε ένα άλλο terminal, με την εντολή: «bash tests/[bash script name]»
    - Αν δεν θέλουμε να τρέξουμε κάποιο test, πάλι σε άλλο terminal τρέχουμε εντολές τύπου:
    ./bin/jobCommander [serverName] [portNum] [jobCommanderInputCommand]
    - Περισσότερες πληροφορίες και παράδειγμα εκτέλεσης στα comments του Makefile.

1. Φτιάχτηκε Queue Structure (από τη πρώτη εργασία) και χρησιμοποιείται για το job buffer. 
Αποτελείται από δύο αρχεία:
    - queue.c: Ο κώδικας των συναρτήσεων, του Queue και του Triplet (struct για τις τριπλέτες των 
    job), τα οποία περνάνε ως Value στα Node του Queue.
    - queue.h: Ο ορισμός των συναρτήσεων και των struct, του Queue και του Triplet.

2. Το αρχείο jobCommander.c κάνει όλες τις λειτουργίες του jobCommander και αυτές μέσω της 
συνάρτησης «int jobCommander(int argc, char *argv[])».
    - Υπάρχει η συνάρτηση «create_job(int argc, char *argv[])», η οποία φτιάχνει ένα μεγάλο
    string, για να αποθηκεύσει το job.
    - Συνδέει τον Commander με τον Server.
    - Στέλνει το job στον Server.
    - Διαβάζει το μήνυμα ή και το output του job, που επέστρεψε ο Server.

3. Το αρχείο jobExecutorServer.c είναι υπεύθυνο για τις συνδέσεις του Server με τους Commanders
και την αποθήκευση/εκτέλεση των jobs που στέλνουν.
    - Δημιουργεί το buffer (Queue) που αποθηκεύονται οι εργασίες.
    - Αρχικοποιεί το struct που κρατάει τις πληροφορίες του Server και το συνδέει με ένα global pointer.
    - Δημιουργεί τα worker threads που είναι σε αναμονή για να εκτελέσουν κάποια διεργασία από το 
    job buffer όταν μπορέσουν.
    - Καλεί τη συνάρτηση «jobExecutorServer()», η οποία προετοιμάζει τον server για να δεχτεί συνδέσεις
    και περιμένει μέχρι να δεχτεί κάποια. 
    - Όταν το τελευταίο συμβεί, δημιουργεί ένα controller thread με detached attribute και είτε εκτελεί 
    το server command, είτε αποθηκεύει το job στο job buffer.
    - Διαχειρίζεται τη διαδικασία του κλεισίματος του Server.

4. Η διαχείριση των commands από τον Commander προς τον Server γίνεται με τα αρχεία:
    - ServerCommands.h: Περιέχει τον ορισμό του struct ServerInfo για το information του Server,
    τις δηλώσεις των συναρτήσεων για κάθε command, δύο βοηθητικές συναρτήσεις για τα worker και 
    controller threads και τον ορισμό ενός βοηθητικού struct για τα arguments των controller thread.
    - ServerCommands.c: Η υλοποίηση σε κώδικα των παραπάνω.
    4.1. Η συνάρτηση «char* commands()»:
        - Ξεκαθαρίζει ποια κατηγορία command καλείται και είτε καλεί την βοηθητική της είτε 
        εκτελεί τη λειτουργία της.
        - Επιστρέφει το μήνυμα που ζητείται για κάθε περίπτωση στον Commander.
    4.2. Η συνάρτηση «Triplet* issueJob()»: 
        - Χρησιμοποιείται για το command «issueJob».
        - Δημιουργεί ένα Triplet για το job και το βάζει στο job buffer.
        - Στέλνει σήμα σε ένα worker thread, ότι ο job buffer δεν είναι άδειος.
        - Επιστρέφει το Triplet.
    4.3. Η συνάρτηση «char* stop_job()»:
        - Χρησιμοποιείται για το command «stop jobID».
        - Αφαιρεί τη ζητούμενη εργασία (αν υπάρχει) από το job buffer.
        - Επιστρέφει μήνυμα ειδοποίησης του τι έκανε.
    4.4. Η συνάρτηση «char* poll(char** tokenized)»:
        - Χρησιμοποιείται για το command «poll».
        - Αν ο job buffer δεν είναι άδειος, βρίσκει το συνολικό μέγεθος των Triplet υπό τη
        μορφή string και κατασκευάζει ένα μεγάλο μήνυμα (string), με όλα τα Triplets του job buffer.
        - Επιστρέφει το μήνυμα.
    4.5. Η συνάρτηση «char* exit_server()»:
        - Χρησιμοποιείται για το command «exit».
        - Είναι υπεύθυνη για την αποστολή μηνύματος για το πρώιμο κλείσιμο του Server, σε όλους τους 
        Commander (αν υπάρχουν) που δεν έχει αρχίσει να εκτελείται το job που έδωσαν στον Server.
        - Είναι υπεύθυνη για τη σήμανση τους κλεισίματος του Server.
    4.6. Η συνάρτηση «void execute_job()»:
        - Είναι η βοηθητική συνάρτηση των worker threads.
        - Ελέγχει αν μπορεί να εκτελέσει ένα job λόγω concurrency και αν ναι, το αφαιρεί από το 
        job buffer και το εκτελεί.
        - Το output του job πηγαίνει σε ένα .output αρχείο, που ύστερα στέλνεται στον Commander που
        είχε στείλει το job στον Server.
    4.7. Η συνάρτηση «void call_commands()»:
        - Είναι η βοηθητική συνάρτηση των controller threads.
        - Είναι υπεύθυνη για να διαβάσει το μήνυμα (job) που έστειλε ο Commander από το socket.
        - Καλεί τη συνάρτηση commands(), που εξηγήθηκε προηγουμένως για να αποφασίσει πώς πρέπει να
        δράσει ο Server.
        - Στέλνει το μήνυμα που επέστρεψε η commands() στο Commander.
