// #include "stdbool.h"
// #include "stack.h"
// #include "raylib.h"

// // Input events that I want to handle:
// // Mouse clicked (left and right)
// // Mouse released (left and right)
// // Mouse moved while down
// // Possibly more...
// // Let's say these are mouse input events.
// // Do I handle them differently than keyboard input events?
// // Some inputs should be consumed, others shouldn't...
// // Some inputs should be sent in priority to specific objects
// // How to interpret an input should be up to the object, but this is order dependent...
// // Example: a rod is clicked and escape is pressed. Does the rod get selected before the game is paused?
// // Better if not.
// // So we should sort input events by priority, before dealing with them?
// // But how can we know their priorities before they're dealt with?..
// // By themselves, inputs don't have any priorities, they get their meaning from objects.
// // Priority of objects then. The "menu" object, say, gets inputs before everyone else.
// // If the input is a big "stop everything" kind of input -- according to the menu object! --,
// // the menu object gets to flush the InputEventQueue.
// // Ok, but again, what's an InputEvent? It should be easy to check what kind of InputEvent and InputEvent is.
// // It should also be easy to get some additionnal info, if relevant, such as the mouse position.
// // What about past input data? Are objects responsible for storing the history of the input they get?
// // This conflicts with the fact that some objects can flush the input queue.
// // Maybe don't flush it, then, just stash it?
// // Doesn't work either...
// // Add a "special" event that the menu object can send, a "pause" event say.
// // Does it use the same queue? Do we send the message through the same channels (some kind of processInput function)?
// // Why not...
// // Why use an event queue again? Well, we don't want to process every event synchronously.
// // Okay, this seems fine.
// // Last but not least: what struct is InputEvent? (is this even the right name anymore?)
// // An enum + some data?
// // Seems reasonable...
// // Yeah, let's do this then.
// // So the input component, processes the input, and uses it to set some relevant flags in the entity
// // SelectableEntity struct, say.
// // Has a rect
// // DI to give the rect to the inputComponent and to the physicsComponent?
// // Give them a pointer, not to the parent object, but to a rect object. Rect with target?
// // Graphics can get the same. And they don't know that they're getting the same. It's fine.
// // Do I give them the info again each time I call the function? Doesn't seem smart, better to just give them a pointer
// // once and for all at the beginning.
// // What are the risks ? I can't think of any right now. So let's go with this.
// // Add a unique id to objects to avoid self-collisions? Seems fine.
// // SignalComponent, ok...
// // GraphicsComponent, fine
// // InputComponent? SelectionComponent?
// // RectComponent? TargetComponent?
// // Don't need an input queue per se, more like an input stack, or just an input list, order matters
// // but I will be "removing" from everywhere and flushing every frame. This is a synchronous affair, too.
// // SelectionInfo is passed to InputComponent during initialization (dependency injection)

// typedef enum MetaInputEventType {
//     MOUSE_EVENT,
//     KEYBOARD_EVENT,
// } MetaInputEventType;

// typedef enum MouseEventType {
//     LEFT_MOUSE_BUTTON_PRESSED,
//     LEFT_MOUSE_BUTTON_RELEASED,
//     MOUSE_MOVED,
// } MouseEventType;

// typedef struct InputEvent {
//     MetaInputEventType metaType;
//     bool consumed;
// } InputEvent;

// // Plusieurs événements différents ont les mêmes données attachées.
// // Tous les événements de type MouseButton ou KeyPressed par exemple.
// // Donc on a envie d'un "meta-type", et puis ensuite d'info plus précise.

// // Mais en fait les événements de type MouseButton n'ont pas tous les mêmes données attachées.
// // Par exemple : un mouvement de souris... Ce serait bien de choper le delta.
// // Alors est-ce que j'ai vraiment envie d'une event queue ? Je me dis que peut-être pas...
// // L'intérêt, c'est de capturer des événements avant qu'ils soient utilisés n'importe comment.
// // C'est aussi d'emballer les événements de sorte à pouvoir abstraire leur source réelle
// // (un fichier history plutôt que les véritables input, par exemple)
// // Est-ce que je peux faire une interface un peu pareil, où j'ajoute des fonctions
// // style "consommer l'input" ?
// // Et donc, pour communiquer à tous les objets qu'on est en pause, par exemple ?
// // Pour qu'ils puissent correctement se marquer comme dé-selectionnés ?
// // "release everything" kinda thing
// // Seems reasonable.
// // Ok, alors, j'ai cet objet qui a des méthodes, du style "IsMouseButtonDown", "GetMousePosition", etc.
// // Mais au lieu de renvoyer des booléens, ils renvoient des "ternaires" : no, yes, yes-but-consumed
// // Ou bien ils renvoient bien des booléens, et puis il y a l'option d'avoir plus d'info si on veut
// // IsMouseButtonPressed(), IsMouseButtonPressedReally(), ConsumeMouseButtonPress()
// // Ouais, why not!
// // Et pour que tout le monde "ferme" sa gueule ? On met à jour les inputs avec des released de partout. Easy peasy.
// // Hmm. Peut-être pas en fait. On a plutôt envie de faire un événement "pause", et chacun gère ça comme il veut.
// // Pause et resume ?
// // Et on les passe comment ces trucs-là ? Par le même canal ?
// // C'est-à-dire : c'est le même objet qui est chargé d'avoir ces infos, mais il faut lui demander si on fait pause
// // quoi. Est-ce que ce serait pas mieux d'appeler une méthode "pause" sur chaque objet ? Si sûrement.
// // Mais là encore, on se dit qu'il va falloir tout prévoir à l'avance...
// // Ça risque d'être un poil galère... 
// // Je veux dire : il va falloir que je prévoie le fait de mettre en pause tout les objets...
// // Après : ça peut être gêré par l'input component de chaque entité, non ?
// // Ben non, y'a aussi des updates qui se passent en dehors, a priori.
// // Hmm, alors : au niveau de l'input component, on gère la dé-selection, tout ce genre de chose,
// // et au niveau de l'update "indépendant" de l'input, on dit juste stop, fini d'updater. Euh, mais il faut encore
// // dessiner, nan ?.. On arrête juste le kinematicComponent ? Okidoki... Et si les update prennent en entrée un delta de temps,
// // ceux qui ne dépendent pas d'un input j'veux dire (voire tous, tant qu'à faire...), on leur file un delta de 0
// // et puis basta ! On est golden. Ok et l'inputComponent du coup, on appelle toujours sa méthode update ?
// // Ui, et lui c'est un des rares inputs qui prend pas le delta de temps en entrée, mais qui a un statut "pause".
// // Ok, alors, un objet il est pausé en entier, pas composant par composant.
// // C'est sa fonction update à lui qui reçoit un delta de temps.
// // Et il le passe aux update de ses composants.
// // Hmm... Est-ce qu'on a pas plutôt envie que chaque composant ait sa manière de récupérer des deltas de temps ?
// // Encore une fois, un coup de DI.
// // Quand on met en pause, on injecte un nouveau "timeProvider" aux composants concernés. Qui ne fait que donner des
// // deltas de 0. Et un nouveau "inputComponent", tant qu'à faire, qui ne fait rien du tout, juste attendre qu'on le
// // sorte de la pause.

// InputEvent NewInputEvent(MetaInputEventType metaType) {
//     return (InputEvent){.metaType = metaType, .consumed = false};
// }

// typedef struct MouseEvent {
//     InputEvent super;
//     MouseEventType type;
//     Vector2 mousePosition;
// } MouseEvent;

// MouseEvent NewMouseEvent(MouseEventType mouseEventType, Vector2 mousePosition) {
//     InputEvent super = NewInputEvent(MOUSE_EVENT);
//     return (MouseEvent){.super = super, .mousePosition = mousePosition, .type = mouseEventType};
// }

// typedef struct InputEventBuffer {
//     Stack stack;
// } InputEventBuffer;

// InputEventBuffer EmptyInputEventBuffer(int capacity) {
//     Stack stack = EmptyStack(capacity, sizeof(InputEvent));
//     return (InputEventBuffer){.stack = stack};
// }

// InputEvent *IndexInputEventBuffer(InputEventBuffer *me, int index) {
//     return (InputEvent*)IndexStack(&me->stack, index);
// }

// void PushInputEventToBuffer(InputEventBuffer *me, InputEvent inputEvent) {
//     PushStack(&me->stack, (void*)&inputEvent);
// }

// void ClearInputEventBuffer(InputEventBuffer *me) {
//     ClearStack(&me->stack);
// }

// typedef struct InputService InputService;

// struct InputService {
//     void (*poll)(InputService *me);
//     InputEventBuffer inputEventBuffer;
// };

// typedef struct PhysicalInputService {
//     InputService super;
// } PhysicalInputService;

// typedef struct PhysicalMouseInputTranslation {
//     bool (*physicalMouseTest)(int button);
//     int button;
//     MouseEventType mouseEventType;
// } PhysicalMouseInputTranslation;

// const PhysicalMouseInputTranslation physicalInputMouseTranslations[] = {
//     (PhysicalMouseInputTranslation){
//         .physicalMouseTest = &IsMouseButtonPressed,
//         .button = MOUSE_BUTTON_LEFT,
//         .mouseEventType = LEFT_MOUSE_BUTTON_PRESSED
//     },

//     (PhysicalMouseInputTranslation){
//         .physicalMouseTest = &IsMouseButtonReleased,
//         .button = MOUSE_BUTTON_LEFT,
//         .mouseEventType = LEFT_MOUSE_BUTTON_RELEASED
//     }

//     // GetMouseDelta
// };

// void PhysicalInputService_Poll(InputService *me) {
//     Vector2 mousePosition = GetMousePosition();
//     if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
//         // PushInputEventToBuffer(&me->inputEventBuffer, );
//     }
// }

// void PhysicalInputService_Construct(PhysicalInputService *service) {
//     service->super.poll = &PhysicalInputService_Poll;
// }