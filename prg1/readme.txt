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
