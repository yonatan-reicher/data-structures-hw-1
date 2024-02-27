#include "Olympicsa1.h"
#include <memory>
#include <sstream>

Olympics::Olympics(){
    // Everything is initialized to be empty.
}

Olympics::~Olympics(){
    // All fields are managed by the data structures.
}

int calculateTeamStrength(const Team& team) {
    if (team.size() % 3 != 0) {
        return 0;
    }

    int strength = 0;
    for (int i = 0; i < NUM_OF_TREES; i++) {
        if (team.m_contestantIds[i].size() == 0) {
            continue;
        }
        strength += team.m_contestantPowers[i].maximumKey().m_strength;
    }
    return strength;
}

output_t<int> getMinOrMaxContestantFromThird(int thirdIndex, const Team& team, bool isMin) {
    if (team.m_contestantIds[thirdIndex].size() == 0) {
        return StatusType::FAILURE;
    }
    if (isMin) {
        return team.m_contestantPowers[thirdIndex].minimumKey().m_id;
    } else {
        return team.m_contestantPowers[thirdIndex].maximumKey().m_id;
    }
}

// Must be O(log #teams + log #contestants)
void Olympics::updateTeamAusterity(int teamId) {
    Team& team = m_teams.get(teamId);

    if (team.size() < 6) {
        team.m_cachedAusterity = 0;
        return;
    }

    int austerity = 0;

    for (int i1 = 0; i1 < 2 * NUM_OF_TREES; i1++) {
        output_t<int> id1 = getMinOrMaxContestantFromThird(i1 % 3, team, i1 < NUM_OF_TREES);
        if (id1.status() != StatusType::SUCCESS) {
            continue;
        }

        remove_contestant_from_team(teamId, id1.ans());
        for (int i2 = 0; i2 < 2 * NUM_OF_TREES; i2++) {
            output_t<int> id2 = getMinOrMaxContestantFromThird(i2 % 3, team, i2 < NUM_OF_TREES);
            if (id2.status() != StatusType::SUCCESS) {
                continue;
            }

            remove_contestant_from_team(teamId, id2.ans());
            for (int i3 = 0; i3 < 2 * NUM_OF_TREES; i3++) {
                output_t<int> id3 = getMinOrMaxContestantFromThird(i3 % 3, team, i3 < NUM_OF_TREES);
                if (id3.status() != StatusType::SUCCESS) {
                    continue;
                }

                remove_contestant_from_team(teamId, id3.ans());
                austerity = std::max(austerity, calculateTeamStrength(team));
                add_contestant_to_team(teamId, id3.ans());
            }
            add_contestant_to_team(teamId, id2.ans());
        }
        add_contestant_to_team(teamId, id1.ans());
    }

    team.m_cachedAusterity = austerity;
}


StatusType Olympics::add_country(int countryId, int medals) {
    try {
        if (countryId <= 0 || medals < 0) {
            return StatusType::INVALID_INPUT;
        }

        if (m_countries.contains(countryId)) {
            return StatusType::FAILURE;
        }

        m_countries.insert(countryId, Country(countryId, medals));

        return StatusType::SUCCESS;
    }
    catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
}

StatusType Olympics::remove_country(int countryId){
    try {
        if (countryId <= 0) {
            return StatusType::INVALID_INPUT;
        }

        if (!m_countries.contains(countryId)) {
            return StatusType::FAILURE;
        }

        const Country& country = m_countries.get(countryId);

        if (!country.canBeDeleted()) {
            return StatusType::FAILURE;
        }

        m_countries.remove(countryId);

        return StatusType::SUCCESS;
    } catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
}

StatusType Olympics::add_team(int teamId,int countryId,Sport sport){
    try {
        if (teamId <= 0 || countryId <= 0) {
            return StatusType::INVALID_INPUT;
        }

        if (!m_countries.contains(countryId)) {
            return StatusType::FAILURE;
        }

        if (m_teams.contains(teamId)) {
            return StatusType::FAILURE;
        }

        Country* country = &m_countries.get(countryId);
        m_teams.insert(teamId, std::move(Team(teamId, sport, country)));
        // This happens after insertion in case of exception.
        country->m_numOfTeams++;

        return StatusType::SUCCESS;
    } catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
}

StatusType Olympics::remove_team(int teamId){
    try {
        if (teamId <= 0) {
            return StatusType::INVALID_INPUT;
        }

        if (!m_teams.contains(teamId)) {
            return StatusType::FAILURE;
        }

        Team& team = m_teams.get(teamId);
        Country* country = team.m_country;

        if (!team.canBeDeleted()) {
            return StatusType::FAILURE;
        }

        m_teams.remove(teamId);
        country->m_numOfTeams--;

        return StatusType::SUCCESS;
    } catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
}

StatusType Olympics::add_contestant(int contestantId ,int countryId,Sport sport,int strength){
    Contestant *contestant = nullptr;
    try {
        Country *country = nullptr;
        if (contestantId <= 0 || countryId <= 0 || strength < 0)
        {
            return StatusType::INVALID_INPUT;
        }
        try
        {
            country = &m_countries.get(countryId);
            contestant = &m_contestants.get(contestantId);
            if(contestant != nullptr){
                return StatusType::FAILURE;
            }
        }
        catch (NotFoundException<int>&)
        {
            if (country == nullptr)
            {
                return StatusType::FAILURE;
            }
        }
        contestant = new Contestant(contestantId, sport, country, strength);

        m_contestants.insert(contestantId, *contestant);
        country->m_numOfContestants++;
        delete contestant;
        return StatusType::SUCCESS;
    }
    catch (std::bad_alloc&){
        delete contestant;
        return StatusType::ALLOCATION_ERROR;
    }
}

StatusType Olympics::remove_contestant(int contestantId){
	if(contestantId <= 0)
    {
        return StatusType::INVALID_INPUT;
    }

    Contestant* contestant;
    try {
        contestant = &m_contestants.get(contestantId);
    }
    catch (NotFoundException<int>&){
        return StatusType::FAILURE;
    }
    if(contestant->canBeDeleted())
    {
        m_contestants.remove(contestantId).m_country->m_numOfContestants--;
        return StatusType::SUCCESS;
    }
    else
    {
        return StatusType::FAILURE;
    }
    // no alloc errors can occur, right?
}

StatusType Olympics::add_contestant_to_team(int teamId,int contestantId){
	if(contestantId <= 0 || teamId <= 0)
    {
        return StatusType::INVALID_INPUT;
    }
    Contestant* contestant;
    Team* team;
    try {
        contestant = &m_contestants.get(contestantId);
        team = &m_teams.get(teamId);
    }
    catch (NotFoundException<int>&){
        return StatusType::FAILURE;
    }
    if(contestant->m_numOfTeams == MAX_NUM_OF_TEAMS
        || contestant->m_sport != team->m_sport
        || contestant->m_country != team->m_country)
    {
        return StatusType::FAILURE;
    }

    contestant->addTeam(team);
    add_contestant_to_team_tree(team, contestant);
    updateTeamAusterity(teamId);

    return StatusType::SUCCESS;
}

StatusType Olympics::remove_contestant_from_team(int teamId,int contestantId){
	if(contestantId <= 0 || teamId <= 0) {
        return StatusType::FAILURE;
    }

    Contestant* contestant;
    Team* team;
    try {
        contestant = &m_contestants.get(contestantId);
        team = &m_teams.get(teamId);
    }
    catch (NotFoundException<int>&){
        return StatusType::FAILURE;
    }

    if (!contestant->removeTeam(team)){
        return StatusType::FAILURE;
    }

    int contestantIdTreeIndex;
    for(int i = 0; i < NUM_OF_TREES; i++)
    {
        if(team->m_contestantIds[i].contains(contestantId)){
            contestantIdTreeIndex = i;
        }
    }

    team->m_contestantIds[contestantIdTreeIndex].remove(contestantId);

    balanceTeamTrees(contestantIdTreeIndex, team);
    updateTeamAusterity(teamId);

    return StatusType::SUCCESS;
}

StatusType Olympics::update_contestant_strength(int contestantId, int change){
    // TODO: Make sure to call this function just after returning!
    try {
        if (contestantId <= 0) {
            return StatusType::INVALID_INPUT;
        }

        if (!m_contestants.contains(contestantId)) {
            return StatusType::FAILURE;
        }

        Contestant& contestant = m_contestants.get(contestantId);
        if (contestant.m_strength + change < 0) {
            return StatusType::FAILURE;
        }

        // Save the contestant's team ids before taking them out.
        int teamIds[MAX_NUM_OF_TEAMS];
        int numOfTeams = contestant.m_numOfTeams;
        for (int i = 0; i < numOfTeams; i++) {
            teamIds[i] = contestant.m_teams[i]->m_id;
        }

        // Take the contestant out of all teams.
        for (int i = 0; i < numOfTeams; i++) {
            remove_contestant_from_team(teamIds[i], contestantId);
        }

        contestant.m_strength += change;

        // Add the contestant back to all teams.
        for (int i = 0; i < numOfTeams; i++) {
            add_contestant_to_team(teamIds[i], contestantId);
            updateTeamAusterity(teamIds[i]);
        }

        return StatusType::SUCCESS;
    } catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
}

output_t<int> Olympics::get_strength(int contestantId){
    if (contestantId <= 0) {
        return StatusType::INVALID_INPUT;
    }

    if (!m_contestants.contains(contestantId)) {
        return StatusType::FAILURE;
    }

    return m_contestants.get(contestantId).m_strength;
}

output_t<int> Olympics::get_medals(int countryId){
    if (countryId <= 0) {
        return StatusType::INVALID_INPUT;
    }

    if (!m_countries.contains(countryId)) {
        return StatusType::FAILURE;
    }

    return m_countries.get(countryId).m_numOfMedals;
}

output_t<int> Olympics::get_team_strength(int teamId){
    try {
        if (teamId <= 0) {
            return StatusType::INVALID_INPUT;
        }

        if (!m_teams.contains(teamId)) {
            return StatusType::FAILURE;
        }

        return calculateTeamStrength(m_teams.get(teamId));
    }
    catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
	return 0;
}

std::unique_ptr<Contestant*[]> getIdArrayOfTeam(const Team& team) {
    std::unique_ptr<Contestant*[]> ret(new Contestant*[team.size()]);

    int written = 0;

    for (int i = 0; i < NUM_OF_TREES; i++) {
        std::unique_ptr<Contestant*[]> ids(team.m_contestantIds[i].toArray());
        for (int j = 0; j < team.m_contestantIds[i].size(); j++) {
            ret[written++] = ids[j];
        }
    }

    return ret;
}

// TODO: Think about: does this work if someone is already in the array? Tree::fromArray maybe doesn't support duplicates!
void combineSortedByIdAndChangeTeam(
    Contestant* arr1[], int size1, Contestant* arr2[], int size2,
    Contestant* ret[],
    Team* team1, Team* team2
) {
    int i = 0;
    int j = 0;
    int k = 0;

    // This is step 3 of unite_teams.

    while (i < size1 || j < size2) {
        // 3.a + 3.b + 3.c
        Contestant* c =
            i == size1 ? arr2[j++] :
            j == size2 ? arr1[i++] :
            arr1[i]->m_id < arr2[j]->m_id ? arr1[i++] : arr2[j++];
        c->m_helperIndex = k;
        ret[k++] = c;

        // 3.d
        c->removeTeam(team2);
        c->addTeam(team1);
    }
}

template <int N>
void combineSortedByPower(
    Contestant** arrays[N],
    int arraySizes[N],
    Contestant* ret[]
) {
    int iRet = 0;
    int arrayIndices[N] = { 0 };

    bool done;
    do {
        // Find firsts from the arrays.
        Contestant* firsts[N] = { 0 };
        for (int i = 0; i < N; i++) {
            firsts[i] =
                arrayIndices[i] < arraySizes[i] ?
                arrays[i][arrayIndices[i]] : nullptr;
        }

        // Find the minimum contestant pointer by strength
        int indexOfMin = -1;
        for (int i = 0; i < N; i++) {
            if (firsts[i] == nullptr) continue;
            if (indexOfMin == -1 || firsts[indexOfMin]->m_strength > firsts[i]->m_strength) {
                indexOfMin = i;
            }
        }

        // Write it! If didn't find one, we read from all the arrays!
        if (indexOfMin != -1) {
            ret[iRet++] = firsts[indexOfMin];
            arrayIndices[indexOfMin]++;
        }
        else done = true;
    } while (!done);
}

int getId(const Contestant* c) {
    return c->m_id;
}

StrengthAndId getStrengthAndId(const Contestant* c) {
    return (*c);
}

StatusType Olympics::unite_teams(int teamId1,int teamId2){
    // TODO: Check for allocation failures.
    if (teamId1 == teamId2 || teamId1 <= 0 || teamId2 <= 0) {
        return StatusType::INVALID_INPUT;
    }

    if (!m_teams.contains(teamId1) || !m_teams.contains(teamId2)) {
        return StatusType::FAILURE;
    }

    Team* team1 = &m_teams.get(teamId1);
    Team* team2 = &m_teams.get(teamId2);

    if (team1->m_sport != team2->m_sport) {
        return StatusType::FAILURE;
    }

    // From here on, I'll label steps according to their numbers as written in
    // the docs.

    std::unique_ptr<Contestant*[]> idsTeam1;
    std::unique_ptr<Contestant*[]> strengthTeam1[NUM_OF_TREES];
    std::unique_ptr<Contestant*[]> idsTeam2;
    std::unique_ptr<Contestant*[]> strengthTeam2[NUM_OF_TREES];

    // Step 1.
    idsTeam1 = getIdArrayOfTeam(*team1);
    idsTeam2 = getIdArrayOfTeam(*team2);
    for (int i = 0; i < NUM_OF_TREES; i++) {
        strengthTeam1[i] = std::unique_ptr<Contestant*[]>(team1->m_contestantPowers[i].toArray());
        strengthTeam2[i] = std::unique_ptr<Contestant*[]>(team2->m_contestantPowers[i].toArray());
    }

    // Step 2.
    int n = team1->size() + team2->size();
    std::unique_ptr<Contestant*[]> arrays[5];
    for (int i = 0; i < 5; i++) {
        arrays[i] = std::unique_ptr<Contestant*[]>(new Contestant*[n]);
        for (int j = 0; j < n; j++) {
            arrays[i][j] = nullptr;
        }
    }

    // Step 3.
    combineSortedByIdAndChangeTeam(
        idsTeam1.get(), team1->size(),
        idsTeam2.get(), team2->size(),
        arrays[0].get(),
        team1, team2
    );

    // Step 4.
    Contestant** strengthArrays[NUM_OF_TREES * 2];
    int arrayLengths[NUM_OF_TREES * 2];
    for (int i = 0; i < NUM_OF_TREES; i++) {
        strengthArrays[i] = strengthTeam1[i].get();
        arrayLengths[i] = team1->m_contestantPowers[i].size();
    }
    for (int i = 0; i < NUM_OF_TREES; i++) {
        strengthArrays[i + NUM_OF_TREES] = strengthTeam2[i].get();
        arrayLengths[i + NUM_OF_TREES] = team2->m_contestantPowers[i].size();
    }
    combineSortedByPower<NUM_OF_TREES * 2>(
        strengthArrays,
        arrayLengths,
        arrays[1].get()
    );

    // Step 5.
    int i1 = 0;
    int i2 = 0;
    int i3 = 0;
    for (int i = 0; i < n; i++) {
        Contestant* c = arrays[1][i];
        if (c->m_helperIndex < n / 3) {
            arrays[2][i1++] = c;
        } else if (c->m_helperIndex < (2 * n) / 3) {
            arrays[3][i2++] = c;
        } else {
            assert (c->m_helperIndex < n);
            arrays[4][i3++] = c;
        }
    }

    // Step 6.
    Tree<int, Contestant*> ids1, ids2, ids3;
    Tree<StrengthAndId, Contestant*> strength1, strength2, strength3;
    ids1 = Tree<int, Contestant*>::fromArray(arrays[0].get(), n / 3, getId);
    ids2 = Tree<int, Contestant*>::fromArray(arrays[0].get() + n / 3, n / 3, getId);
    ids3 = Tree<int, Contestant*>::fromArray(arrays[0].get() + (2 * n) / 3, n - (2 * n) / 3, getId);
    // This is to make sure rounding errors dont cause us to miss elements  ---^^^
    strength1 = Tree<StrengthAndId, Contestant*>::fromArray(arrays[2].get(), n / 3, getStrengthAndId);
    strength2 = Tree<StrengthAndId, Contestant*>::fromArray(arrays[3].get(), n / 3, getStrengthAndId);
    strength3 = Tree<StrengthAndId, Contestant*>::fromArray(arrays[4].get(), n - (2 * n) / 3, getStrengthAndId);

    // Step 7.
    team1->m_contestantIds[0] = std::move(ids1);
    team1->m_contestantIds[1] = std::move(ids2);
    team1->m_contestantIds[2] = std::move(ids3);
    team1->m_contestantPowers[0] = std::move(strength1);
    team1->m_contestantPowers[1] = std::move(strength2);
    team1->m_contestantPowers[2] = std::move(strength3);

    // Step 8. 
    updateTeamAusterity(teamId1);

    // Step 9.
    remove_team(teamId2);

	return StatusType::SUCCESS;
}

int getTeamPoints(const Team& team) {
    return calculateTeamStrength(team) + team.m_country->m_numOfMedals;
}

StatusType Olympics::play_match(int teamId1,int teamId2){
    if (teamId1 == teamId2 || teamId1 <= 0 || teamId2 <= 0) {
        return StatusType::INVALID_INPUT;
    }

    if (!m_teams.contains(teamId1) || !m_teams.contains(teamId2)) {
        return StatusType::FAILURE;
    }

    Team& team1 = m_teams.get(teamId1);
    Team& team2 = m_teams.get(teamId2);

    if (team1.m_sport != team2.m_sport) {
        return StatusType::FAILURE;
    }

    int pointsTeam1 = getTeamPoints(team1);
    int pointsTeam2 = getTeamPoints(team2);

    if (pointsTeam1 == pointsTeam2) {
        return StatusType::FAILURE;
    }

    Country& winner = pointsTeam1 > pointsTeam2 ? *team1.m_country : *team2.m_country;
    winner.m_numOfMedals++;

    return StatusType::SUCCESS;
}

output_t<int> Olympics::austerity_measures(int teamId){
    try {
        if (teamId <= 0) {
            return StatusType::INVALID_INPUT;
        }

        if (!m_teams.contains(teamId)) {
            return StatusType::FAILURE;
        }

        return m_teams.get(teamId).m_cachedAusterity;
    }
    catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
}

void Olympics::add_contestant_to_team_tree(Team *team, Contestant *contestant) {
    if(team->m_contestantIds[0].size() == 0 || contestant->m_id < team->m_contestantIds[0].maximumKey()){
        team->m_contestantIds[0].insert(contestant->m_id, contestant);
        team->m_contestantPowers[0].insert(getStrengthAndId(contestant), contestant);
        balanceTeamTrees(0, team);
    }
    else if(team->m_contestantIds[1].size() == 0 || contestant->m_id < team->m_contestantIds[1].maximumKey()){
        team->m_contestantIds[1].insert(contestant->m_id, contestant);
        team->m_contestantPowers[1].insert(getStrengthAndId(contestant), contestant);
        balanceTeamTrees(1, team);
    }
    else {
        team->m_contestantIds[2].insert(contestant->m_id, contestant);
        team->m_contestantPowers[2].insert(getStrengthAndId(contestant), contestant);
        balanceTeamTrees(2, team);
    }

}

void Olympics::balanceTeamTrees(int changedTreeIndex, Team *team) {
    balanceTwoTrees(team, changedTreeIndex, (changedTreeIndex + 1) % 3);
    balanceTwoTrees(team, changedTreeIndex, (changedTreeIndex + 2) % 3);

    if(0 == team->m_contestantIds[changedTreeIndex].size()) {
        sortRoots(changedTreeIndex, team);
    }
}

void Olympics::moveContestantBetweenTeamTrees(Team *team, int srcTree, int destTree) {
    assert(team->m_contestantIds[srcTree].size() > 0);
    assert(team->m_contestantPowers[srcTree].size() > 0);

    if(srcTree == destTree){
        return;
    }

    Contestant* contestantToMove =
            srcTree < destTree
            ? team->m_contestantIds[srcTree].remove(team->m_contestantIds[srcTree].maximumKey())
            : team->m_contestantIds[srcTree].remove(team->m_contestantIds[srcTree].minimumKey());
    team->m_contestantIds[destTree].insert(contestantToMove->m_id, contestantToMove);

    team->m_contestantPowers[srcTree].remove(getStrengthAndId(contestantToMove));
    team->m_contestantPowers[destTree].insert(getStrengthAndId(contestantToMove), contestantToMove);
}

std::string Olympics::prettyPrint() const {
    std::stringstream ss;
    ss << "Countries:" << std::endl;
    ss << m_countries << std::endl;
    ss << "Teams:" << std::endl;
    ss << m_teams << std::endl;
    ss << "Contestants:" << std::endl;
    ss << m_contestants << std::endl;
    return ss.str();
}

void Olympics::balanceTwoTrees(Team *team, int firstTreeId, int secondTreeId) {
    if(firstTreeId > secondTreeId){
        std::swap(firstTreeId, secondTreeId);
    }

    if(team->m_contestantIds[firstTreeId].size() - team->m_contestantIds[secondTreeId].size() == 2) {
        if(secondTreeId - firstTreeId == 2){
            for(int i = firstTreeId; i < secondTreeId; i++)
            {
                moveContestantBetweenTeamTrees(team, i, i + 1);
            }
        }
        else
        {
            moveContestantBetweenTeamTrees(team, firstTreeId, secondTreeId);
        }
    }
    else if(team->m_contestantIds[firstTreeId].size() - team->m_contestantIds[secondTreeId].size() == -2) {
        if(secondTreeId - firstTreeId == 2){
            for(int i = secondTreeId; i > firstTreeId; i--)
            {
                moveContestantBetweenTeamTrees(team, i, i - 1);
            }
        }
        else
        {
            moveContestantBetweenTeamTrees(team, secondTreeId, firstTreeId);
        }
    }
}

void Olympics::sortRoots(int emptiedTreeIndex, Team *team) {
    for(int i = emptiedTreeIndex + 1; i < NUM_OF_TREES; i++)
    {
        if(team->m_contestantIds[i].size() != 0) {
            moveContestantBetweenTeamTrees(team, i, i - 1);
        }
    }
}


/*
    X - done   / - half done    .  started
     __________________________________________________________________________                
    |            Method             |  status |             notes              |
    |-------------------------------|---------|--------------------------------|
	| add_country                   |    X    |                                |
	| remove_country                |    X    |                                |
	| add_team                      |    X    |                                |
	| remove_team                   |    X    |                                |
	| add_contestant                |    X    |                                |
	| remove_contestant             |    X    |                                |
	| add_contestant_to_team        |    X    |                                |
	| remove_contestant_from_team   |    /    |                                |
	| update_contestant_strength    |    X    |                                |
	| get_strength                  |    X    |                                |
	| get_medals                    |    X    |                                |
	| get_team_strength             |    X    |                                |
	| unite_teams                   |    /    |   Need to handle contestants that are in both teams   |
	| play_match                    |    /    |                                |
	| austerity_measures            |    X    |                                |

TODO:
1. Whatsapp tests
2. Run tests with valgrind on the server
3. Make sure duplicates are handled correctly in unite_team
*/
