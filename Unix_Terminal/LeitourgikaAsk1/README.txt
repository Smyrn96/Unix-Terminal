EMMANOUIL SMYRNAKIS CSD3504


Στην άσκηση 1 που παραδίδω έχω υλοποιήσει τα εξής:
	
α)Την εντολή cd
	
β)Την εντολή exit
	
γ)Την συνάρτηση Pipe()
	
δ)Τα Redirections
	
ε)Τις ls εντολές
	
στ)Τις cat εντολές



Όλα τα παραπάνω τα υλοποιώ με την type_prompt η οποία ανοίγει ενα καινούργιο prompt με το όνομα χρήστη και το pathname,  με την read_command() όπου διαβάζω από το πληκτρολόγιο 
όλες τις εντολές και εκτελώ αντίστοιχες εντολές όπου καλώ αντίστοιχα τις print_directory() για ls -l εντολή και ls -r,
print_contents() για ls εντολή ,changeDir() για cd εντολή, redirection() για redirect, και pipe_function() για εντολη pipe().Τελος εχω την main() οπου γινεται fork().
