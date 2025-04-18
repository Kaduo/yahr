from copy import deepcopy
from itertools import permutations
import seaborn as sns
import polars as pl
from scipy.stats import ttest_ind

TABLET_WIDTH = 1024
TABLET_HEIGHT = 600

UNIT_ROD_HEIGHT = 30


class Rod:
    def __init__(self, n=-1):
        self.n = n


def is_raw_grouping_consistent(grouping):
    seen = [rod.n for rod in grouping[0]]
    for i in range(1, 4):
        for j in [rod.n for rod in grouping[i]]:
            if j in seen:
                return False
        seen += [rod.n for rod in grouping[i]]
    return True


def _dist(grouping1, grouping2):
    res = 0
    for bin in range(4):
        for rod in grouping1[bin]:
            if rod.n not in [rod.n for rod in grouping2[bin]]:
                res += 1
    return res


def dist(grouping1, grouping2):
    # if not (is_raw_grouping_consistent(grouping1) and is_raw_grouping_consistent(grouping2)):
    #     return None

    current_grouping = deepcopy(grouping1)

    # best_perm = deepcopy(current_grouping)
    best_dist = _dist(current_grouping, grouping2)

    for bin_permutation in permutations(range(4)):
        current_grouping = deepcopy(grouping1)
        current_grouping = [current_grouping[i] for i in bin_permutation]
        current_dist = _dist(current_grouping, grouping2)
        if current_dist < best_dist:
            best_dist = current_dist

    return best_dist


ideal_grouping_ns = [[2, 4, 8], [3, 6, 9], [1, 7], [5, 10]]


def score_grouping(grouping):
    return dist(grouping, [[Rod(k) for k in group] for group in ideal_grouping_ns])


def which_zone(x, y):
    if y <= TABLET_HEIGHT / 2 - UNIT_ROD_HEIGHT:
        # print("out y :", y)
        return None
    if x < TABLET_WIDTH / 2:
        if y < 3 * TABLET_HEIGHT / 4:
            return 0
        else:
            return 1
    elif TABLET_WIDTH / 2 <= x:
        if y < 3 * TABLET_HEIGHT / 4:
            return 2
        else:
            return 3


def get_grouping_from_file(filename):
    grouping = [[], [], [], [], 0]
    rods = []
    can_be_three = False
    current_rod_is_done = False
    current_offset = None

    with open(filename) as f:
        for k, l in enumerate(f):
            line = l.split()
            if line[0] == "rod":
                rods.append(Rod())
                current_rod_is_done = False
            elif current_rod_is_done:
                current_offset = None
                can_be_three = False
                continue
            elif line[0] == "rect":
                x = float(line[1])
                y = float(line[2])
                if which_zone(x, y) is not None:
                    grouping[which_zone(x, y)].append(rods[-1])
                else:
                    grouping[4] += 1
                width = float(line[4])
                if width != 3 * UNIT_ROD_HEIGHT:
                    rods[-1].n = int(width // UNIT_ROD_HEIGHT)
                    current_rod_is_done = True
                else:
                    can_be_three = True
            elif line[0] == "color":
                g = int(line[2])
                if g == 255:
                    if can_be_three:
                        can_be_three = False
                    else:
                        rods[-1].n = 1
                        current_rod_is_done = True
                else:
                    current_rod_is_done = True
                if g == 46:
                    rods[-1].n = 2
                elif g == 222:
                    rods[-1].n = 3
                elif g == 74:
                    rods[-1].n = 4
                elif g == 211:
                    rods[-1].n = 5
                elif g == 163:
                    rods[-1].n = 6
                elif g == 64:
                    rods[-1].n = 7
                elif g == 82:
                    rods[-1].n = 8
                elif g == 144:
                    rods[-1].n = 9
                elif g == 157:
                    rods[-1].n = 10

            elif line[0] == "offset":
                current_offset = int(line[1])

            elif line[0] == "period":
                i = 0
                if current_offset == 0:
                    i = 2
                elif current_offset == 27:
                    i = 1
                period = int(line[1])
                if period == 4:
                    rods[-1].n = [1, 7][i]
                elif period == 20:
                    rods[-1].n = [2, 4, 8][i]
                elif period == 70:
                    rods[-1].n = [3, 6, 9][i]
                elif period == 200:
                    rods[-1].n = [5, 10][i]

    # print("hmm : ", filename, grouping[4])
    # print("ohwouw : ", filename, len([x for x in grouping[:4] if len(x) > 0]))
    return grouping


def get_groupings_in_folder(folder_path):
    groupings = []
    for i in range(0, 10):
        filename = f"{folder_path}/session_{i}.rods"
        groupings.append(get_grouping_from_file(filename))

    return groupings


def display_grouping(grouping):
    for i, group in enumerate(grouping):
        if i < 4:
            print(f"group {i} : {[rod.n for rod in group]}")
        else:
            print("nombre de réglettes en dehors de tout groupe :", group)


def get_scores_for_user(user_id):
    scores_visual = []
    scores_haptic = []
    for i in range(0, 10):
        filename = f"data/pre-test/results/haptic/user_{user_id}/session_{i}.rods"
        grouping = get_grouping_from_file(filename)

        scores_haptic.append(score_grouping(grouping))
    for i in range(0, 10):
        filename = f"data/pre-test/results/visual/user_{user_id}/session_{i}.rods"
        grouping = get_grouping_from_file(filename)
        scores_visual.append(score_grouping(grouping))

    return scores_haptic, scores_visual


def get_scores_for_total_user(user_id):
    scores_total = []
    for i in range(0, 10):
        filename = f"data/pre-test/results/combined/user_{user_id}/session_{i}.rods"
        grouping = get_grouping_from_file(filename)
        scores_total.append(score_grouping(grouping))

    return scores_total


def get_durations_from_files(filename):
    res = []
    with open(filename, "r") as f:
        f.readline()
        for line in f:
            res.append(float(line))
    return res


class Grouping:
    def __init__(self, filename):
        self.raw_grouping = get_grouping_from_file(filename)
        self.inner = [[rod.n for rod in group] for group in self.raw_grouping[:4]]

    def nb_inconsistencies(self):
        res = 0
        seen = deepcopy(self.inner[0])
        for i in range(1, 4):
            for n in self.inner[i]:
                if n in seen:
                    res += 1
            seen += deepcopy(self.inner[i])
        return res

    # A grouping is consistent if identical rods are grouped together
    def is_consistent(self):
        return self.nb_inconsistencies() == 0

    def score(self):
        return score_grouping(self.raw_grouping)


class User:
    def __init__(self, user_id, mode):
        self.user_id = user_id
        self.mode = mode
        self.groupings = []
        for i in range(5):
            self.groupings.append(
                Grouping(
                    f"data/experiment/results/{mode}/{user_id}/problem_{i}_end.rods"
                )
            )
        for i in range(15, 20):
            self.groupings.append(
                Grouping(
                    f"data/experiment/results/{mode}/{user_id}/problem_{i}_end.rods"
                )
            )

        # try:
        #     self.durations = get_durations_from_files(f"data/experiment-test/results/{mode}/user_{user_id}/user{user_id}_durations")
        # except FileNotFoundError:
        #     self.durations = None
        self.durations = None

    def scores(self):
        res = []
        for grouping in self.groupings:
            res.append(grouping.score())

        return res

    def inconsistency_score(self):
        res = 0
        tot = 0
        for i in range(10):
            for j in range(i + 1, 10):
                tot += 1
                res += dist(
                    self.groupings[i].raw_grouping, self.groupings[j].raw_grouping
                )
        if tot == 0:
            None
        else:
            return res / tot

    def score(self):
        return ma_mean([grouping.score() for grouping in self.groupings])

    def nb_inconsistent_groupings(self):
        res = 0
        for grouping in self.groupings:
            if not grouping.is_consistent():
                res += 1

        return res


def get_users(mode):
    users = []
    if mode == "haptics":
        user_ids = ["ecma1", "ecma2", "hugo", "louis"]
    elif mode == "no_haptics":
        user_ids = ["ecma3", "emma", "enzo", "mathis", "romain"]
    else:
        raise f"Unkown mode : {mode}"

    for id in user_ids:
        users.append(User(id, mode))

    return users


def ma_sum(l):
    res = 0
    for k in l:
        if k is not None:
            res += k
    return res


def ma_len(l):
    res = 0
    for k in l:
        if k is not None:
            res += 1
    return res


def ma_mean(l):
    if l is None:
        return None
    if ma_len(l) > 0:
        return ma_sum(l) / ma_len(l)
    else:
        return None


if __name__ == "__main__":
    modes = ["haptics", "no_haptics"]
    for mode in modes:
        print(mode)
        users = get_users(mode)
        for user in users:
            print(
                "user_id : ",
                user.user_id,
                "| score : ",
                user.score(),
                " | inconsistency score : ",
                user.inconsistency_score(),
                " | duration : ",
                ma_mean(user.durations),
                " | nb_inconsistent_groupings : ",
                user.nb_inconsistent_groupings(),
            )

        print(
            "mean score : ",
            ma_mean([user.score() for user in users]),
            "| mean inconsistency score : ",
            ma_mean([user.inconsistency_score() for user in users]),
            "| mean duration : ",
            ma_mean([ma_mean(user.durations) for user in users]),
            "| mean nb_inc_groupings : ",
            ma_mean([user.nb_inconsistent_groupings() for user in users]),
        )

    # print(ttest_ind([user.score() for user in get_users("combined")], [user.score() for user in get_users("visual")]))
    # print(ttest_ind([user.inconsistency_score() for user in get_users("combined")], [user.inconsistency_score() for user in get_users("haptic")]))

# if __name__ == "__main__":

#     user_ids = [500, 501, 502, 600, 700, 800, 801, 20, 21, 22]
#     total_user_ids = [61, 150, 151, 152, 153, 351, 352, 353, 200]
#     total_user_scores = []
#     haptic_user_scores = []
#     visual_user_scores = []

#     for user_id in user_ids:
#         user_scores = get_scores_for_user(user_id)
#         haptic_user_scores.append(user_scores[0])
#         visual_user_scores.append(user_scores[1])
#     for user_id in total_user_ids:
#         total_user_scores.append(get_scores_for_total_user(user_id))

#     haptic_df = pl.DataFrame(haptic_user_scores, schema=[f"user_{user_id}" for user_id in user_ids])
#     visual_df = pl.DataFrame(visual_user_scores, schema=[f"user_{user_id}" for user_id in user_ids])
#     total_df = pl.DataFrame(total_user_scores, schema=[f"user_{user_id}" for user_id in total_user_ids])
#     print(haptic_df.describe())
#     print(visual_df.describe())
#     print(total_df.describe())

#     haptic_df.write_csv("haptic_df.csv")
#     visual_df.write_csv("visual_df.csv")
#     total_df.write_csv("total_df.csv")
