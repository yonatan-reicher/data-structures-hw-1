#include "Olympicsa1.h"
#include <memory>

Olympics::Olympics(){

}

Olympics::~Olympics(){

}
	
StatusType Olympics::add_country(int countryId, int medals){
	return StatusType::FAILURE;
}
	
StatusType Olympics::remove_country(int countryId){
	return StatusType::FAILURE;
}

StatusType Olympics::add_team(int teamId,int countryId,Sport sport){
	return StatusType::FAILURE;
}

StatusType Olympics::remove_team(int teamId){
	return StatusType::FAILURE;
}
	
StatusType Olympics::add_contestant(int contestantId ,int countryId,Sport sport,int strength){
	return StatusType::FAILURE;
}
	
StatusType Olympics::remove_contestant(int contestantId){
	return StatusType::FAILURE;
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
	return 0;
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
