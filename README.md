# Chat

The Scavenge and Survive chat system, provides these methods of communicating:

* Local: within a 40m radius, simulates talking/shouting to nearby players
* Action: local /me chat for roleplaying
* Global: server-wide chat
* Radio: server-wide chat where you can only talk to people on the same frequency

Not a totally isolated package yet, does depend on the SS Administration package.

## Installation

Simply install to your project:

```bash
sampctl package install ScavengeSurvive/chat
```

Include in your code and begin using the library:

```pawn
#include <chat>
```

## Testing

To test, simply run the package:

```bash
sampctl package run
```

Then connect to `localhost:7777`.
