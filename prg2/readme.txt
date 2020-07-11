VAIHE 2:

Käytettyjen algoritmien valinnan perusteet:

Käytin harjoitustyössä seuraavia algoritmeja:
journey_any: BFS (journey_least_stops suoraan)
journey_least_stops: BFS
journey_with_cycle: DFS
journey_shortest_distance: A*
journey_earliest_arrival: Dijkstra

Kaikki nämä valitsin oikeastaan suoraan luentojen, harjoitusten ja muun kurssin
materiaalin perusteella siten, että mikäli materiaalissa mainittiin tai pystyin
itse ajattelemaan algoritmin olevan soveltamiskelpoinen ko. funktioon, otin sen
käyttöön. Ainoa "valinta" oli A*:n käyttäminen journey_shortest_distance:ssa,
jossa ajattelin sen olevan tehokkaampi kuin Dijkstra sellaisenaan.

Päätietorakenteiden valinnan perusteet:

Tietorakenteet:
----------------------------------------------------------------------------------
// Type for a single routes data
struct RouteData {
    std::vector<StopID> stops = {};
    std::vector<StopData *> stopPtrs = {};
    std::vector<std::vector<Time>> trips = {};
};

// Type for storing all routes
using RoutesDS = std::unordered_map<RouteID, RouteData>;

Selite:
Structissa on tallennettuna yksittäisen reitin tiedot: pysäkkien ID:t, pointterit
pysäkkeihin sekä reitillä kulkevat matkat. Tämä rakenne valikoitui suurilta osin
route_stops- ja all_routes-funktioiden helpottamiseksi: reittiä lisätessä ovat sen
pysäkit helposti saatavilla ja lisättävissä tällaiseen muotoon, ja samoin tällaisesta
formaatista ne on helppo myöskin palauttaa julkisen rajapinnan vaatimassa muodossa.

RoutesDS-rakenteen valitsin sen takia, että useimmiten reittejä haetaan niiden ID:llä,
jolloin unordered_map on tehokkain valinta.
----------------------------------------------------------------------------------
struct StopData {
    StopID id;
    Name name;
    Coord coordinates;
    float distanceFromZero;
    RegionID region = NO_REGION;
    std::unordered_multimap<RouteID, Edge> routesTo = {};

    // Variables for Dijkstra, A*, BFS and DFS-algorithms
    Colour colour = WHITE;              // Colour
    int d = INF;                        // Cumulative distance at the stop
    int de = INF;                       // A*-distance: journey so far + shortest possible distance
    StopData * pii = NULL;              // The stop where the trail comes from
    RouteID dRoute = NO_ROUTE;          // The route from which the trail comes from
    Time departureFromPrevious = INF;   // The departure time from previous stop
    Time arrivalTime = INF;             // The arrival time to the stop
};

Selite:
Lisäsin eri polkualgoritmien vaatimat parametrit kaikki suoraan yksittäisten pysäkkien
tietorakenteeseen. En nähnyt järkeväksi tehdä omaa graafia (tai sen solmuja) mallintavaa
tietorakennetta, joka kuitenkin olisi esittänyt yksittäistä pysäkkiä. Tätä kautta pystyin
helposti käyttämään pysäkkikohtaisia parametreja suoraan kopioimatta niitä, kun polkualgoritmien
solmut olivat pointtereita yksittäisten pysäkkien dataan (esim. StopData * v). Kaikkia
parametreja ei luonnollisesti kaikissa kohdissa/algoritmeissa tarvitse, mutta on helpompi säilyttää ne
mukana vrt. pelailla pysäkkien datan ja erillisen graafitietorakenteen välillä.

Yleisesti pyrin käyttämään mahdollisimman pitkälle pointtereita ja iteraattoreita
parametrien käsittelyssä ja välttämään täten niiden kopioinnin.

routesTo-muuttujan unordered_multimap-pohjainen rakenne valikoitui sen takia, että journey_earliest_arrival-
funktiossa tuli käsitellä kunkin reitin mahdolliset matkat reittikohtaisesti. Täten lisätessä
matkoja (add_trip) tietorakenteeseen, oli tehokkaampaa lisätä (lisäys alla) unordered_multimap:iin
yksittäisen matkan kaari reitin id:n perusteella verrattuna esim. vektorista for-loopilla
etsimiseen.

"stop->routesTo[routeid].tripsTo.push_back(tripEdge);"

Nimenomaan multimap valikoitui add_walking_connections-funktion tarpeiden myötä. Siinä luon
kullekin pysäkille 5 uutta "reittiä", joiden kaikkien RouteID on NO_ROUTE. Päällekkäisyyksiä ei
oikeiden reittien ollessa kyseessä multimapin takia tule, sillä add_route-metodi tarkastaa ettei
samalla RouteID:llä varustettuja reittejä voi lisätä.

----------------------------------------------------------------------------------
// Type for route graph edge
struct RouteEdge {
    RouteID route;
    StopData * nextStop;
    int distance;
    std::vector<TripEdge> tripsTo = {};
    bool walking = false;
};

// Type for trip graph edge
struct TripEdge {
    Time departureTime;
    Time nextArrivalTime;
};

Selite:
RouteEdge ilmentää nimensä mukaisesti reittien muodostaman graafin kaarta, ja
TripEdge on taas yksittäistä vuoroväliä kuvastava kaari, joka kuuluu aina tietylle
reitille ja pysäkille. Tein sisäkkäiset tietorakenteet (TripEdget sisältävä vektori
kuuluu aina tietyn RouteEdgen sisään) jotta journey_shortest_distance-funktiossa
pystyttäisiin käymään aina tietylle kahden pysäkin väliselle reitille juuri tuolle
pysäkkivälille ja reitille kuuluvat lähtö- sekä saapumisajat läpi siten, että ulommassa
loopissa käydään läpi pysäkiltä toiselle sisältävät reitit ja sisemmässä loopissa ko.
reitin sisältämät lähtö- ja saapumisajat. Näistä lähtö- sekä saapumisajoista valitaan
reitin paras ja sitä verrataan saman pysäkkivälin muihin reitteihin. (Pahoittelen,
toivottavasti selityksestä saa selvää, en tiedä pystynkö paremmin tätä formalisoimaan :D .)
----------------------------------------------------------------------------------
Muiden tietorakenteiden valinnan perusteet:
----------------------------------------------------------------------------------


Tietorakenteet:
----------------------------------------------------------------------------------
// Type for Dijkstra priority queue pair
using PqPair = std::pair<int, StopData *>;

// Declare the priority queue
std::priority_queue<PqPair, std::deque<PqPair>, std::greater<PqPair>> pq;

Selite:
Prioriteettijonoa ja sen yksiköitä varten en löytänyt suoraan tämän valmiimpaa
ratkaisua. Luennollakin mainitun ongelman prioriteettijonon priorisoinnista (eli
sen priorisoidessa suuret numerot pienten edelle) ratkaisin std::greater-funktiolla
vertailussa sekä luomalla PqPair-tietotyypille <>-operaattorit.

Valitsin dequen vektorin sijasta sen ollessa luentokalvon taulukon mukaan tehokkaampi
lisäys muualle-operaatiossa (deque: vakiaikainen ensimmäiselle alkiolle, muuten O(n),
vector: O(n)). Suuren suurta merkitystä sillä ei taida olla mutta niistä kahdesta
valitsin täten paremman.


----------------------------------------------------------------------------------
----------------------------------------------------------------------------------
----------------------------------------------------------------------------------
----------------------------------------------------------------------------------
Tässä vielä vaihe 1:n readme, mikäli jokin asia sieltä mietityttää (tuskin käyttöä
mutta laitanpa mukaan silti).

VAIHE 1:

Päätietorakenteiden valinnan perusteet:

Tietorakenteet:
StopsDS = std::unordered_map<StopID, StopData>;
RegionsDS = std::unordered_map<RegionID, RegionData>;

// Type for a single stop's data
struct StopData {
    Name name;
    Coord coordinates;
    float distanceFromZero;
    RegionID region = NO_REGION;
};

// Type for a single region's data
struct RegionData {
    Name name;
    RegionID parentRegion = NO_REGION;
    std::vector<RegionID> childRegions = {};
};

Selite:
Kahta ylempänä olevaa ja niiden alkioita haetaan useimmiten ID:llä, esimerkiksi
kaikkien pysäkkien ja alueiden ominaisuudet haetaan niiden ID:n mukaan, ja
toisaalta myös esim. pysäkkejä lisätessä ja muita muokkauksia tehdessä
tulee aina tarkistaa, onko ko. pysäkkiä ja/tai aluetta jo olemassa tai onko
jossain niiden ominaisuudessa toisen arvo tjsp.

--> Täten valinnaksi unordered_map, jossa kaikista paras asymptoottinen
tehokkuus tietyn alkion (tietyllä ID:llä) etsinnässä. Lisäksi valitsin mapin
setin sijasta, koska ajattelin että pysäkit ja alueet sisältävät vaihtelevan
määrän muuta dataa, jonka saisi kätevästi Structiin. Täten ko. Structiin pääsisi
kätevästi kummankin tietotyypin ID:n avulla. Muut toteutukset ja esim. väliaikais-
tietorakenteet on valittu näiden valintojen ehdoilla mahdollisimman tehokkaiksi.


Muiden tietorakenteiden valinnan perusteet:

Tietorakenteet:
StopID maxCoordStop = NO_STOP;
StopID minCoordStop = NO_STOP;

Selite:
Minimi- ja maksimikoordinaattien etsintä on vaivailloisempaa ja tehottomampaa
tehdä aina erikseen vrt. että päivitetään niiden arvoja jatkuvasti muuttujaan.
Ainakaan itse en keksinyt erikseen tehtävään min- ja max-koordinaattien etsintään
lineaarista parempaa ratkaisua, joten päädyin tähän ratkaisuun.

Tietorakenne:
StopNames = std::unordered_map<Name, std::vector<StopID>>;

Selite:
Pysäkkien hakeminen nimillä oli melko raskasta suoraan StopsDS-tietorakenteesta,
joten päätin tehdä vielä erikseen mapin, josta kyetään hakemaan tietynnimisten
pysäkkien ID:t nimellä. Toki harjoitustyöohjeessa oli että ko. find_stops-
operaatiota haetaan harvoin, mutta katsoin että sen suoritus oli ilman ylimääräistä
tietorakennetta niin hidas että päätin tehdä vielä erikseen sitä varten uuden.
Lienee makuasia onko se järkevää, sillä mm. change_stop_name-, add_stop- ja
clear_all-funktioita ainakin jouduttiin ko. tietorakenteen takia muokkaamaan ja
saavutetttu hyöty harvoin kutsuttavassa funktiossa ei välttämättä ole kovin
merkittävä.
