#include "Olympicsa1.h"
#include <memory>

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
        strength += team.m_contestantPowers[i].maximumKey().m_strength;
    }
    return strength;
}

output_t<int> getStrongestContestantFromThird(int thirdIndex, const Team& team) {
    if (team.m_contestantIds[thirdIndex].size() == 0) {
        return StatusType::FAILURE;
    }
    return team.m_contestantPowers[thirdIndex].maximumKey().m_id;
}

// Must be O(log #teams + log #contestants)
void Olympics::updateTeamAusterity(int teamId) {
    Team& team = m_teams.get(teamId);

    int austerity = 0;

    for (int i1 = 0; i1 < NUM_OF_TREES; i1++) {
        output_t<int> id1 = getStrongestContestantFromThird(i1, team);
        if (id1.status() != StatusType::SUCCESS) {
            continue;
        }

        remove_contestant_from_team(teamId, id1.ans());
        for (int i2 = i1; i2 < NUM_OF_TREES; i2++) {
            output_t<int> id2 = getStrongestContestantFromThird(i2, team);
            if (id2.status() != StatusType::SUCCESS) {
                continue;
            }

            remove_contestant_from_team(teamId, id2.ans());
            for (int i3 = i2; i3 < NUM_OF_TREES; i3++) {
                output_t<int> id3 = getStrongestContestantFromThird(i3, team);
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
    try {
        if (contestantId <= 0 || countryId <= 0 || strength < 0)
        {
            return StatusType::INVALID_INPUT;
        }
        Country *country = nullptr;
        Contestant *contestant = nullptr;
        try
        {
            country = &m_countries.get(countryId);
            contestant = &m_contestants.get(contestantId);
        }
        catch (NotFoundException<int>&)
        {
            if (country == nullptr || contestant != nullptr)
            {
                return StatusType::FAILURE;
            }
        }
        // TODO: This is never deleted --v
        // Maybe don't use a pointer?
        contestant = new Contestant(contestantId, sport, country, strength);

        m_contestants.insert(contestantId, *contestant);
        country->m_numOfContestants++;
        return StatusType::SUCCESS;
    }
    catch (std::bad_alloc&){
        return StatusType::ALLOCATION_ERROR;
    }
}
	
StatusType Olympics::remove_contestant(int contestantId){
	if(contestantId <= 0)
    {
        return StatusType::INVALID_INPUT;
    }

    try {
        Contestant contestant = m_contestants.get(contestantId);
        if(contestant.canBeDeleted())
        {
            m_contestants.remove(contestantId);
            contestant.m_country->m_numOfContestants--;
            return StatusType::SUCCESS;
        }
        else
        {
            return StatusType::FAILURE;
        }
    }
    catch (NotFoundException<int>&){
        return StatusType::FAILURE;
    }
    // no alloc errors can occur, right?
}
	
StatusType Olympics::add_contestant_to_team(int teamId,int contestantId){
	return StatusType::FAILURE;
}

StatusType Olympics::remove_contestant_from_team(int teamId,int contestantId){
	return StatusType::FAILURE;
}

StatusType Olympics::update_contestant_strength(int contestantId ,int change){
	return StatusType::FAILURE;
}

output_t<int> Olympics::get_strength(int contestantId){
    return 0;
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
        strengthTeam1[i] = std::unique_ptr<Contestant*[]>(team2->m_contestantPowers[i].toArray());
        strengthTeam2[i] = std::unique_ptr<Contestant*[]>(team2->m_contestantPowers[i].toArray());
    }

    // Step 2.
    int n = team1->size() + team2->size();
    std::unique_ptr<Contestant*[]> arrays[5];
    for (int i = 0; i < 5; i++) {
        arrays[i] = std::unique_ptr<Contestant*[]>(new Contestant*[n]);
        for (int j = 0; j < team1->size(); j++) {
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
    // TODO: Update the strength of Team 1
    
    // Step 9.
    // TODO: Delete Team 2 (Use the method so the team's country also updates
    // it's counter).

	return StatusType::SUCCESS;
}

StatusType Olympics::play_match(int teamId1,int teamId2){
	return StatusType::FAILURE;
}

output_t<int> Olympics::austerity_measures(int teamId){
	return 0;
}

