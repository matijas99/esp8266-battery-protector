# Uređaj za zaštitu 12V baterije

Ovaj uređaj štiti vašu 12V **automobilsku bateriju** (ili sličnu **olovnu bateriju**) od dubokog pražnjenja. Kontinuirano prati napon baterije i automatski isključuje potrošač kada napon padne ispod 11V, čime sprječava oštećenje baterije.

## LED indikatori

### 🟢 Zelena LED lampica
- **Upaljena**: Baterija je u redu, napon je iznad praga od 11V, potrošač je uključen
- **Treperi (blinka)**: Napon baterije je porastao iznad 12.8V što znači da je započelo punjenje baterije, uređaj čeka 60 sekundi prije automatskog ponovnog uključivanja potrošača
- **Ugašena**: Uređaj je aktivirao zaštitu baterije i isključio potrošača

### 🔴 Crvena LED lampica
- **Upaljena**: Uređaj je aktivirao zaštitu, napon baterije je ispod 11V, potrošač je isključen
- **Upaljena + zelena treperi**: Napon baterije je porastao iznad 12.8V, uređaj čeka 60 sekundi prije automatskog ponovnog uključivanja
- **Ugašena**: Sve je u redu, potrošač je spojen

## Gumb za testiranje

Uređaj ima gumb za testiranje i ručno upravljanje:

- **Pritisnite dok je potrošač uključen** (zelena LED upaljena): Simulira pad napona ispod 11V i isključuje potrošača. Korisno za testiranje zaštitne funkcije.

- **Pritisnite dok je potrošač isključen** (crvena LED upaljena): Odmah ponovno uključuje uređaj, zaobiđući automatsko čekanje. Korisno za ručno vraćanje u rad ili testiranje.

## Automatsko ponovno uključivanje

Nakon što uređaj isključi potrošač zbog niskog napona:
- Uređaj kontinuirano prati napon baterije
- **Automatski se ponovno uključuje samo ako napon poraste iznad 12.8V** (što znači da se baterija počela puniti)
- Nakon što napon prijeđe 12.8V, uređaj čeka 60 sekundi prije ponovnog uključivanja potrošača
- Ako napon ponovno padne ispod 11V nakon uključivanja, uređaj se odmah ponovno isključuje

## Važne napomene

⚠️ **U slučaju greške**: Uređaj je dizajniran tako da u slučaju kvara ili gubitka napajanja automatski zadrži potrošač uključenim. Ovo osigurava kontinuitet napajanja potrošača, ali riskira zdravlje baterije ako se problem ne riješi.

⚠️ **Napon baterije**: Uređaj radi s 12V automobilskim baterijama ili sličnim olovnim baterijama. Ne koristite s drugim tipovima baterija bez provjere kompatibilnosti.

## Osnovno rješavanje problema

| Problem | Rješenje |
|---------|----------|
| Crvena LED upaljena, potrošač ne radi | Napon baterije je ispod 11V. Napunite bateriju kako bi se podigao njen napon |
| Zelena LED ne svijetli | Provjerite napajanje uređaja i kablove |
| Potrošač se ne uključuje nakon punjenja | Pričekajte da napon poraste iznad 12.8V, zatim čekajte 60 sekundi |
| Alarm se čuje | Uređaj je aktivirao zaštitu zbog niskog napona baterije |
| Nista ne svijetli i uređaj je "mrtav" | Provjerite napajanje uređaja i da li je ulazni napon iznad 5V. Ako i dalje ne radi, moguće da je došlo do kvara u kojem slučaju potrošač ostaje uključen. |

## Tehničke specifikacije

- **Napon isključivanja**: 11.0V
- **Napon uključivanja**: 12.8V
- **Kašnjenje uključivanja**: 60 sekundi
- **Minimalni napon rada**: 5V DC
- **Maksimalna struja**: 10A
