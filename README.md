# Projet – Mini Rootkit Windows

## Fonctionnalités du rootkit

Le rootkit est un driver qui expose le device `\\.\MiniRootkit` pour communiquer avec l’espace utilisateur _via_ des IOCTLs.

Il implémente les IOCTLs suivants, visibles dans le fichier [`shared/ioctl.h`](./src/shared/ioctl.h) :

- `IOCTL_ROOT_PID` : Élève les droits du processus donné (il devient `NT AUTHORITY\SYSTEM`)
- `IOCTL_HIDE_PID` : Cache le processus donné de la liste des processus
- `IOCTL_SHOW_PID` : Montre le processus donné (annule l’action de `IOCTL_HIDE_PID`)

Pour chacun de ces IOCTLs, le PID du processus cible est passé _via_ un buffer de 4 octets dans `lpInBuffer`, qui est un pointeur vers un entier de type `LONG` (32 bits signé).
Le rootkit n’utilise pas le buffer de sortie `lpOutBuffer`.

Il est possible de passer deux valeurs spéciales pour le PID, qui sont calculées par le driver :

- `PID_SELF` (`0`) : le processus appelant
- `PID_PARENT` (`-1`) : le processus parent de l’appelant (par exemple pour élever la console qui exécute le programme utilisateur)

Beaucoup d’informations sur les actions effectuées par le rootkit sont logguées avec `DbgPrint` et peuvent être visualisées avec DebugView. Le rootkit utilise le préfixe `[MiniRootkit]` pour identifier ses messages.

Les processus cachés sont enregistrés dans un tableau de 256 éléments (`HIDDEN_PID_LIST_SIZE`) dans la mémoire du rootkit, donc si le driver est désactivé ou rechargé, la liste des processus cachés est perdue et il sera impossible de les afficher à nouveau.

## Exemple d’utilisation : programme compagnon

Le programme compagnon `companion.exe` permet d’interagir avec le rootkit. Il utilise les IOCTLs décrits ci-dessus pour proposer une interface simple en ligne de commande :

```text
Usage: companion.exe <function> [args]

Available functions:
  root_pid <pid> - Elevate a process by PID
  hide_pid <pid> - Hide a process by PID
  show_pid <pid> - Show a previously hidden process
  start_process <cmdline> - Start a new process with admin privileges and hide it immediately

PID 0 refers to the current process
PID -1 refers to the parent process
```

Exemples d’utilisation du programme compagnon :

```powershell
# Élève les droits du processus avec le PID 1234
.\companion.exe root_pid 1234

# Élève le processus courant (companion.exe) en affichant ses droits avant et après
.\companion.exe root_pid 0

# Élève le processus parent (cmd.exe ou powershell.exe)
.\companion.exe root_pid -1

# Crée une nouvelle console administrateur interactive qui exécute whoami au lancement
.\companion.exe start_process 'cmd /k "C: & whoami /user"'

# Cache le processus de PID 1234
.\companion.exe hide_pid 1234

# Montre le processus de PID 1234 après l’avoir caché
.\companion.exe show_pid 1234
```

## Compilation et chargement du driver et du programme compagnon

Plusieurs scripts sont fournis pour faciliter la compilation et le chargement du driver et du programme compagnon.

### Préréquis

**ATTENTION** : Les structures opaques (disponibles dans [`structures.h`](./src/rootkit/structures.h)) proviennent de **Windows 11 24H2** et ne sont pas forcément compatibles avec d’autres versions de Windows.

Pour compiler le driver, il faut installer le [Windows Driver Kit (WDK)](https://learn.microsoft.com/en-us/windows-hardware/drivers/download-the-wdk).
Le projet a été testé avec la version 10.0.26100.3323 de l’EWDK, qui est la dernière à l’heure actuelle (mai 2025).

Pour le programme compagnon, il faut le [Windows SDK](https://developer.microsoft.com/windows/downloads/windows-sdk/) et les [Build Tools de Visual Studio 2022](https://visualstudio.microsoft.com/downloads/#tools-for-visual-studio-2022-family).
Le projet a été testé avec la version 10.0.22621.5040 du SDK et la version 17.14.0 des Build Tools (cl.exe version 19.44.35207.1).

### Compilation du driver

Un script cmd est fourni pour compiler le driver : [`build_rootkit.cmd`](./build_rootkit.cmd).
Il charge automatiquement l'environnement de compilation de l’EWDK si besoin, en supposant que l’ISO a été montée sur `D:\`.
Il utilise ensuite `MSBuild` pour compiler le driver à partir du fichier de projet [`mini_rootkit.vcxproj`](./mini_rootkit.vcxproj).

Le driver est compilé en mode `Debug` et le fichier .sys final est placé dans le dossier `x64\Debug`.

### Chargement du driver

Le driver peut être chargé avec le script [`load.cmd`](./load.cmd), qui utilise `sc.exe` pour le charger et le démarrer après l’avoir copié dans le dossier `C:\Windows\System32\drivers`.
Si le driver est déjà chargé, il sera relancé.

Il y a aussi un script [`unload.cmd`](./unload.cmd) pour l’arrêter et le supprimer du système.

### Compilation du programme compagnon

Le script [`build_companion.cmd`](./build_companion.cmd) compile le programme compagnon à l’aide de `cl.exe`. Il charge automatiquement l’environnement de compilation de Visual Studio si nécessaire, en utilisant l’outil de Microsoft `vswhere.exe` pour localiser le chemin de `vcvars64.bat`.
Les fichiers de compilation intermédiaires sont placés dans le dossier `obj` et le binaire final est placé dans le répertoire racine du projet.
