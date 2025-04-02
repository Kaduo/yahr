#include "raylib.h"
#include "rodfactory.h"
#include "haptic.h"
#include "stdlib.h"
#include "consts.h"
#include "time.h"
#include "serialize.h"
#include "session.h"

const uint16_t PERIODS[] = {10, 30, 90, 180};

Rods *MakeFullRods(const StandardRodFactory *rodFactory, int nbRodsPerLength[10]) {
    Rods *rods = malloc(sizeof(Rods));

    int nbRodsTotal = 0;
    for (int l = 0; l <NB_ROD_LENGTHS; l ++) {
        nbRodsTotal += nbRodsPerLength[l];
    }
    *rods = EmptyRods(nbRodsTotal);

    for (int l = 0; l < NB_ROD_LENGTHS; l++) {
        int nbRods = nbRodsPerLength[l];
        for (int i = 0; i < nbRods; i++) {
            AppendRod(rods, CreateRod(rodFactory, (l+1), 0, 0, ROD_COLOR_FOR_LENGTH[l], RodSignalForLength(l)));
        }
    }

    return rods;
}


Rods *MakeColorAndLengths(const StandardRodFactory *rodFactory, int nbRodsPerLength[10]) {
    Rods *rods = malloc(sizeof(Rods));

    int nbRodsTotal = 0;
    for (int l = 0; l <NB_ROD_LENGTHS; l ++) {
        nbRodsTotal += nbRodsPerLength[l];
    }
    *rods = EmptyRods(nbRodsTotal);

    for (int l = 0; l < NB_ROD_LENGTHS; l++) {
        int nbRods = nbRodsPerLength[l];
        for (int i = 0; i < nbRods; i++) {
            AppendRod(rods, CreateRod(rodFactory, (l+1), 0, 0, ROD_COLOR_FOR_LENGTH[l], (Signal){.amplitude = 0, .pulses = 128, .signalShape = SignalShape_NO_SIGNAL}));
        }
    }

    return rods;
}



Rods *HapticAndLengthButNoColor(const StandardRodFactory *rodFactory, int nbRodsPerLength[10]) {
    Rods *rods = malloc(sizeof(Rods));

    int nbRodsTotal = 0;
    for (int l = 0; l <NB_ROD_LENGTHS; l ++) {
        nbRodsTotal += nbRodsPerLength[l];
    }
    *rods = EmptyRods(nbRodsTotal);

    for (int l = 0; l < NB_ROD_LENGTHS; l++) {
        int nbRods = nbRodsPerLength[l];
        for (int i = 0; i < nbRods; i++) {
            AppendRod(rods, CreateRod(rodFactory, (l+1), 0, 0, WHITE, RodSignalForLength(l)));
        }
    }

    return rods;
}

Rods *OnlyHaptics(const StandardRodFactory *rodFactory, int nbRodsPerLength[10]) {
    Rods *rods = malloc(sizeof(Rods));

    int nbRodsTotal = 0;
    for (int l = 0; l <NB_ROD_LENGTHS; l ++) {
        nbRodsTotal += nbRodsPerLength[l];
    }
    *rods = EmptyRods(nbRodsTotal);

    for (int l = 0; l < NB_ROD_LENGTHS; l++) {
        int nbRods = nbRodsPerLength[l];
        for (int i = 0; i < nbRods; i++) {
            AppendRod(rods, CreateRod(rodFactory, 3, 0, 0, ROD_COLOR_FOR_LENGTH[0], RodSignalForLength(l)));
        }
    }

    return rods;
}

Rods *OnlyColors(const StandardRodFactory *rodFactory, int nbRodsPerLength[10]) {
    Rods *rods = malloc(sizeof(Rods));

    int nbRodsTotal = 0;
    for (int l = 0; l <NB_ROD_LENGTHS; l ++) {
        nbRodsTotal += nbRodsPerLength[l];
    }
    *rods = EmptyRods(nbRodsTotal);

    for (int l = 0; l < NB_ROD_LENGTHS; l++) {
        int nbRods = nbRodsPerLength[l];
        for (int i = 0; i < nbRods; i++) {
            AppendRod(rods, CreateRod(rodFactory, 3, 0, 0, ROD_COLOR_FOR_LENGTH[0], RodSignalForLength(l)));
        }
    }

    return rods;
}

Rods *ColorsAndHapticsButNoLength(const StandardRodFactory *rodFactory, int nbRodsPerLength[10]) {
    Rods *rods = malloc(sizeof(Rods));

    int nbRodsTotal = 0;
    for (int l = 0; l <NB_ROD_LENGTHS; l ++) {
        nbRodsTotal += nbRodsPerLength[l];
    }
    *rods = EmptyRods(nbRodsTotal);

    for (int l = 0; l < NB_ROD_LENGTHS; l++) {
        int nbRods = nbRodsPerLength[l];
        for (int i = 0; i < nbRods; i++) {
            AppendRod(rods, CreateRod(rodFactory, 3, 0, 0, ROD_COLOR_FOR_LENGTH[l], RodSignalForLength(l)));
        }
    }

    return rods;
}
// Ok, il me faut : une marge sur les côtés, une marge en bas, et une marge en haut. Il faut : que je dis aux zones jusqu'où elles vont en haut,
// et au réglettes jusqu'où elles ont le droit d'aller en bas.

// Je veux : lancer un exécutable sur grand pc, lancer un exécutable sur la tablette.
// Qui est le serveur et qui est le client ? Ça paraît plus logique de faire de la tablette le serveur, nan ?
// Comme ça, euh... chais pas en fait. C'est pas très important je crois. Mais oui je pense que la tablette devrait être le serveur.
// Elle a toujours la même ip, elle bouge pas, l'ordi peut-être qu'il bouge.

// Ok, alors, le code qui enchaîne les sessions écoute ce que l'ordi lui envoie. Est-ce que j'envoie juste des "next slide please" ?
// Ou bien carrément toute la spec ?
// Envoyer toute la spec c'est plus lent, en théorie (en pratique j'imagine qu'il ne doit pas y avoir une grande différence...)
// Mais ça permet d'éviter les problèmes de synchro.
// Donc ça me paraît être une pas si mauvaise idée.
// Et ça découple bien les deux programmes
// Et l'ordi est vraiment le client dans ce contexte, "vas-y mets moi les réglettes là où je veux"
// Ok, donc c'est décidé.
// Est-ce que je renvoie les positions terminées au serveur ? Là encore ça paraît pas mal niveau découplage.
// Et niveau synchro. Le serveur envoie les infos, le client peu t en faire ce qu'il veut direct, pas besoin de faire une phase de synchro après
// Ok, bon.
// Est-ce que j'enregistre tous les inputs ?
// Hmm, pour cette expérience je pense que c'est pas nécessaire.
// Plus important : comment j'analyse les résultats ? Il me faut une manière de visualiser les signaux haptiques des réglettes.
// Juste un nombre accroché à chaque réglette ? Genre son groupe ?
// Ou alors quand je renvoie le résultat de l'expé, je stocke juste : telle réglette dans telle zone, pas d'histoire de pixels.
// Ça paraît raisonnable...
// Alors on fait comme ça.
// Ensuite, j'ai le pb de la taille des zones.
// Bon, ça c'est pas trop dur je pense...

int main(void) {
    srand((unsigned int)time(NULL));
    InputService inputService = NewPhysicalInputService();
    PhysicalHapticDriver hapticDriver = NewPhysicalHapticDriver(&inputService);
    HapticService hapticService = NewHapticService((HapticDriver*)&hapticDriver);
    StandardRodFactory rodFactory = {.inputService = (InputService*)&inputService, .hapticService = &hapticService, .rodUnitHeight = 30};


    for (int i =0; i<10; i ++) {
        int nbRodsPerLength[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
        nbRodsPerLength[rand() % NB_ROD_LENGTHS] += 1;
        nbRodsPerLength[rand() % NB_ROD_LENGTHS] += 1;
        nbRodsPerLength[rand() % NB_ROD_LENGTHS] += 1;
        nbRodsPerLength[rand() % NB_ROD_LENGTHS] += 1;
        Rods *rods = MakeFullRods(&rodFactory, nbRodsPerLength);
        ShuffleRods(rods, 0, TABLET_WIDTH - 10*rodFactory.rodUnitHeight, 0, TABLET_HEIGHT/2. - rodFactory.rodUnitHeight);
        char filename[60] = {0};

        sprintf(filename, "data/pre-test/sessions/colors+haptic+length/session_%d.rods", i);
        FILE *f = fopen(filename, "w");
        WriteRods(f, rods);
        fclose(f);
    }

    // for (int i =0; i<10; i ++) {
    //     int nbRodsPerLength[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    //     nbRodsPerLength[rand() % NB_ROD_LENGTHS] += 1;
    //     nbRodsPerLength[rand() % NB_ROD_LENGTHS] += 1;
    //     nbRodsPerLength[rand() % NB_ROD_LENGTHS] += 1;
    //     nbRodsPerLength[rand() % NB_ROD_LENGTHS] += 1;
    //     Rods *rods = OnlyColors(&rodFactory, nbRodsPerLength);
    //     ShuffleRods(rods, 0, TABLET_WIDTH - 10*rodFactory.rodUnitHeight, 0, TABLET_HEIGHT/2. - rodFactory.rodUnitHeight);
    //     char filename[60] = {0};

    //     sprintf(filename, "data/pre-test/sessions/colors/session_%d.rods", i);
    //     FILE *f = fopen(filename, "w");
    //     WriteRods(f, rods);
    //     fclose(f);
    // }

    // for (int i =0; i<10; i ++) {
    //     int nbRodsPerLength[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    //     nbRodsPerLength[rand() % NB_ROD_LENGTHS] += 1;
    //     nbRodsPerLength[rand() % NB_ROD_LENGTHS] += 1;
    //     nbRodsPerLength[rand() % NB_ROD_LENGTHS] += 1;
    //     nbRodsPerLength[rand() % NB_ROD_LENGTHS] += 1;
    //     Rods *rods = OnlyHaptics(&rodFactory, nbRodsPerLength);
    //     ShuffleRods(rods, 0, TABLET_WIDTH - 10*rodFactory.rodUnitHeight, 0, TABLET_HEIGHT/2. - rodFactory.rodUnitHeight);
    //     char filename[60] = {0};

    //     sprintf(filename, "data/pre-test/sessions/haptic/session_%d.rods", i);
    //     FILE *f = fopen(filename, "w");
    //     WriteRods(f, rods);
    //     fclose(f);
    // }


    // for (int i =0; i<10; i ++) {
    //     int nbRodsPerLength[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    //     nbRodsPerLength[rand() % NB_ROD_LENGTHS] += 1;
    //     nbRodsPerLength[rand() % NB_ROD_LENGTHS] += 1;
    //     nbRodsPerLength[rand() % NB_ROD_LENGTHS] += 1;
    //     nbRodsPerLength[rand() % NB_ROD_LENGTHS] += 1;
    //     Rods *rods = ColorsAndHapticsButNoLength(&rodFactory, nbRodsPerLength);
    //     ShuffleRods(rods, 0, TABLET_WIDTH - 10*rodFactory.rodUnitHeight, 0, TABLET_HEIGHT/2. - rodFactory.rodUnitHeight);
    //     char filename[60] = {0};

    //     sprintf(filename, "data/pre-test/sessions/colors+haptic/session_%d.rods", i);
    //     FILE *f = fopen(filename, "w");
    //     WriteRods(f, rods);
    //     fclose(f);
    // }


//     for (int i =0; i<10; i ++) {
//         int nbRodsPerLength[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
//         nbRodsPerLength[rand() % NB_ROD_LENGTHS] += 1;
//         nbRodsPerLength[rand() % NB_ROD_LENGTHS] += 1;
//         nbRodsPerLength[rand() % NB_ROD_LENGTHS] += 1;
//         nbRodsPerLength[rand() % NB_ROD_LENGTHS] += 1;
//         Rods *rods = MakeColorAndLengths(&rodFactory, nbRodsPerLength);
//         ShuffleRods(rods, 0, TABLET_WIDTH - 10*rodFactory.rodUnitHeight, 0, TABLET_HEIGHT/2. - rodFactory.rodUnitHeight);
//         char filename[60] = {0};

//         sprintf(filename, "data/pre-test/sessions/colors+length/session_%d.rods", i);
//         FILE *f = fopen(filename, "w");
//         WriteRods(f, rods);
//         fclose(f);
//     }
//    return 0;

}