# Projet – Mini Rootkit Windows <!-- omit in toc -->

- [Fonctionnalités du rootkit](#fonctionnalités-du-rootkit)
  - [Manipulation des processus](#manipulation-des-processus)
  - [Manipulation des fichiers](#manipulation-des-fichiers)
- [Exemple d’utilisation : programme compagnon userland](#exemple-dutilisation--programme-compagnon-userland)
- [Compilation et chargement du driver et du programme compagnon](#compilation-et-chargement-du-driver-et-du-programme-compagnon)
  - [Préréquis](#préréquis)
  - [Compilation du driver](#compilation-du-driver)
  - [Chargement du driver](#chargement-du-driver)
  - [Compilation du programme compagnon](#compilation-du-programme-compagnon)

## Fonctionnalités du rootkit

Le rootkit est un driver qui expose le device `\\.\MiniRootkit` pour communiquer avec l’espace utilisateur _via_ des IOCTLs.

Beaucoup d’informations sur les actions effectuées par le rootkit sont logguées avec `DbgPrint` et peuvent être visualisées avec [DebugView](https://learn.microsoft.com/sysinternals/downloads/debugview). Il utilise le préfixe `[MiniRootkit]` pour identifier ses messages.

Il implémente les IOCTLs suivants, visibles dans le fichier [`shared/rootkit_uapi.h`](./src/shared/rootkit_uapi.h) :

- `IOCTL_ROOT_PID` (`0x2600`) : Élève les droits du processus donné (il devient `NT AUTHORITY\SYSTEM`)
- `IOCTL_HIDE_PID` (`0x2601`) : Cache le processus donné de la liste des processus
- `IOCTL_SHOW_PID` (`0x2602`) : Montre le processus donné (annule l’action de `IOCTL_HIDE_PID`)
- `IOCTL_HIDE_FILE` (`0x2603`) : Cache les fichiers correspondant au motif donné par le buffer d’entrée de type `HIDDEN_FILE`
- `IOCTL_SHOW_FILE` (`0x2604`) : Retire le motif donné de la liste des fichiers cachés (le fichier redeviendra visible)

> Notes :
>
> - Pour `IOCTL_SHOW_FILE`, le motif doit être exactement le même que celui utilisé pour cacher le fichier (la chaîne, sa longueur et le type de motif).
>   Cela permet d’avoir des motifs avec des chaînes identiques par exemple pour un préfixe et un suffixe, mais de les traiter indépendamment.
> - Les motifs sont comparés de façon sensible à la casse, même si le système de fichiers est insensible à la casse (il serait facile de modifier ce comportement en remplaçant les fonctions de comparaison de chaînes, par exemple `wcsncmp` par `_wcsnicmp`).
> - Lors de l’ajout d’un élément dans un tableau de choses à cacher (PID ou motif), aucune vérification n’est faite pour savoir si l’élément est déjà présent, donc attention aux doublons (ce n’est pas un problème en soi lors de la vérification de la présence d’un élement, mais s’il y a _N_ versions d’un même élément, il faudra _N_ appels pour le retirer complètement).

### Manipulation des processus

Pour les 3 premiers IOCTLs, le PID du processus cible est passé _via_ un buffer de 4 octets dans `lpInBuffer`, qui est un pointeur vers un entier de type `LONG` (32 bits signé).
Le rootkit n’utilise pas le buffer de sortie `lpOutBuffer`.

Il est possible de passer deux valeurs spéciales pour le PID, qui sont calculées par le driver :

- `PID_SELF` (`0`) : le processus appelant
- `PID_PARENT` (`-1`) : le processus parent de l’appelant (par exemple pour élever la console qui exécute le programme utilisateur)

Pour masquer un processus, l’entrée correspondante est retirée de la liste doublement chaînée pointée par le champ `ActiveProcessLinks` de sa structure `EPROCESS`.
Pour rendre un processus visible à nouveau, il est réinséré après l’entrée de la liste chaînée correspondant au processus appelant le rootkit (_"requestor"_).

Pour élever les privilèges d’un processus, le rootkit modifie simplement le champ `Token` de la structure `EPROCESS` du processus cible pour y copier le token du processus `System` (qui a toujours le PID `4` et donc facile à localiser avec `PsLookupProcessByProcessId`).

Les processus cachés sont enregistrés dans un tableau de 256 éléments (`HIDDEN_PID_LIST_SIZE`) dans la mémoire du rootkit, donc si le driver est désactivé ou rechargé, la liste des processus cachés est perdue et il sera impossible de les afficher à nouveau.

### Manipulation des fichiers

Pour les IOCTLs `IOCTL_HIDE_FILE` et `IOCTL_SHOW_FILE`, le motif est une chaîne de 30 caractères maximum (`WCHAR`).
Le type de motif pour la structure `HIDDEN_FILE` peut être :

- `MATCH_TYPE_EXACT` (`0`) : Le nom de fichier doit **correspondre** exactement au motif
- `MATCH_TYPE_SUBSTRING` (`1`) : Le nom de fichier doit **contenir** le motif
- `MATCH_TYPE_PREFIX` (`2`) : Le nom de fichier doit **commencer** par le motif
- `MATCH_TYPE_SUFFIX` (`3`) : Le nom de fichier doit **se terminer** par le motif

Les fichiers sont cachés grâce à un MiniFilter qui écoute les post-opérations `IRP_MJ_DIRECTORY_CONTROL` avec la fonction mineure `IRP_MN_QUERY_DIRECTORY`, qui est utilisée pour lister le contenu d’un répertoire.
Le buffer de sortie est parcouru pour retirer les fichiers qui correspondent à un des motifs cachés.

Il existe plusieurs classes pour les informations de fichiers (correspondant chacune à une structure différente), celles qui nous intéressent sont :

| Nom de classe                    | Structure associée             |
| -------------------------------- | ------------------------------ |
| `FileBothDirectoryInformation`   | `FILE_BOTH_DIR_INFORMATION`    |
| `FileIdBothDirectoryInformation` | `FILE_ID_BOTH_DIR_INFORMATION` |
| `FileFullDirectoryInformation`   | `FILE_FULL_DIR_INFORMATION`    |
| `FileIdFullDirectoryInformation` | `FILE_ID_FULL_DIR_INFORMATION` |
| `FileDirectoryInformation`       | `FILE_DIRECTORY_INFORMATION`   |
| `FileNamesInformation`           | `FILE_NAMES_INFORMATION`       |

Les structures ont des offsets différents pour leurs champs, ce qui empêche de faire une fonction générique simple. On utilise donc un tableau d’éléments de type `FILE_INFORMATION_DEFINITION` (défini dans `ntifs.h`), qui sont des structures qui enregistrent l’ID de classe ainsi que les offsets des champs dont nous avons besoin : `NextEntryOffset`, `FileNameLength` et `FileName`.

Par chance, le header `ntifs.h` définit également des macros pour les offsets des champs de toutes les classes d’informations de fichiers sous la forme d’une initialisation de structure, ce qui nous permet de les utiliser directement pour remplir le tableau (les macros ont le nom de la classe avec le suffixe `Definition`, par exemple `FileBothDirectoryInformationDefinition`).

La fonction `PostDirectoryControl` cherche les bons offsets pour la classe d’informations à partir de l’ID de classe présent dans le champ `FileInformationClass`, puis les utilise pour traiter de façon générique le buffer de sortie.

Contrairement aux IOCTLs relatifs aux processus, le tableau pour les motifs cachés ne contient que 64 éléments (pour une taille totale de 4096 octets, soit une page mémoire), et si le driver est désactivé ou rechargé, la liste des motifs sera également perdue, mais les fichiers cachés redeviendront visibles (car le MiniFilter ne sera plus actif, et aucune structure du kernel n’est modifiée).

## Exemple d’utilisation : programme compagnon userland

Le programme compagnon `companion.exe` permet d’interagir avec le rootkit. Il utilise les IOCTLs décrits ci-dessus pour proposer une interface simple en ligne de commande :

```text
Usage: companion.exe <function> [args]

Available functions:
  root_pid <pid> - Elevate a process by PID
  hide_pid <pid> - Hide a process by PID (max 256 hidden PIDs)
  show_pid <pid> - Show a previously hidden process
  start_process <cmdline> - Start a new process with admin privileges and hide it immediately
  hide_file <exact|substring|prefix|suffix> <pattern> - Hide files matching 'pattern' (max 64 patterns)
  show_file <exact|substring|prefix|suffix> <pattern> - Remove pattern from hidden file list

PID 0 refers to the current process
PID -1 refers to the parent process

Pattern matching types:
  exact     - Match the entire string
  substring - Match anywhere in the string
  prefix    - Match the beginning of the string
  suffix    - Match the end of the string
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

# Cache les fichiers dont le nom contient est exactement "secret.txt"
.\companion.exe hide_file exact secret.txt

# Cache les fichiers dont le nom contient "_secret_" (par exemple "my_secret_file.pdf")
.\companion.exe hide_file substring _secret_

# Cache les fichiers dont le nom commence par "secret_" (par exemple "secret_pic.png")
.\companion.exe hide_file prefix secret_

# Cache les fichiers dont le nom se termine par ".secret" (par exemple "document.docx.secret")
.\companion.exe hide_file suffix .secret

# Remontre les fichiers cachés dont le nom est exactement "secret.txt"
.\companion.exe show_file exact secret.txt

# Ne fonctionne pas : essaye de remontrer un fichier caché dont le nom est exactement "_secret_",
# alors qu’il a été caché avec le motif en mode "substring"
.\companion.exe show_file exact _secret_
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

Le driver peut être chargé avec le script [`load.cmd`](./load.cmd), qui utilise `sc.exe` pour le charger et le démarrer après l’avoir copié dans le dossier `C:\Windows\System32\drivers`. Les clés registres nécessaires au MiniFilter sont également créées.
Si le driver est déjà chargé, il sera relancé.

Il y a aussi un script [`unload.cmd`](./unload.cmd) pour l’arrêter et le supprimer du système.

### Compilation du programme compagnon

Le script [`build_companion.cmd`](./build_companion.cmd) compile le programme compagnon à l’aide de `cl.exe`. Il charge automatiquement l’environnement de compilation de Visual Studio si nécessaire, en utilisant l’outil de Microsoft `vswhere.exe` pour localiser le chemin de `vcvars64.bat`.
Les fichiers de compilation intermédiaires sont placés dans le dossier `obj` et le binaire final est placé dans le répertoire racine du projet.
