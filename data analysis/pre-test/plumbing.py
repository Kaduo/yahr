from copy import deepcopy
from itertools import permutations
import seaborn as sns
import polars as pl

TABLET_WIDTH = 1024
TABLET_HEIGHT = 600

UNIT_ROD_HEIGHT = 30

class Rod:
    def __init__(self, n=-1):
        self.n = n

ideal_grouping_ns = [[2, 4, 8], [3, 6, 9], [1,7], [5,10]]

def loss(grouping):
    res = 0
    for bin in range(4):
        for rod in grouping[bin]:
            if rod.n not in ideal_grouping_ns[bin]:
                res += 1
    return res

def score_grouping(grouping):
    current_grouping  = deepcopy(grouping)

    # best_perm = deepcopy(current_grouping)
    best_loss = loss(current_grouping)

    for bin_permutation in permutations(range(4)):
        current_grouping = deepcopy(grouping)
        current_grouping = [current_grouping[i] for i in bin_permutation]
        current_loss = loss(current_grouping)
        if current_loss < best_loss:
            # best_perm = deepcopy(current_grouping)
            best_loss = current_loss

    return best_loss + grouping[4]

def which_zone(x, y):
    if y <= TABLET_HEIGHT/2 - UNIT_ROD_HEIGHT:
        # print("out y :", y)
        return None
    if x < TABLET_WIDTH/2:
        if y < 3*TABLET_HEIGHT/4:
            return 0
        else:
            return 1
    elif TABLET_WIDTH/2 <= x:
        if y < 3*TABLET_HEIGHT/4:
            return 2
        else:
            return 3
    
def get_grouping_from_file(filename):
    grouping = [[],[],[],[], 0]
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
                if which_zone(x,y) is not None:
                    grouping[which_zone(x, y)].append(rods[-1])
                else:
                    grouping[4] += 1
                width = float(line[4])
                if width != 3*UNIT_ROD_HEIGHT:
                    rods[-1].n = int(width//UNIT_ROD_HEIGHT)
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
                    rods[-1].n = [3,6,9][i]
                elif period == 200:
                    rods[-1].n = [5,10][i]

    # print("hmm : ", filename, grouping[4])
    # print("ohwouw : ", filename, len([x for x in grouping[:4] if len(x) > 0]))
    return grouping

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


if __name__ == "__main__":

    user_ids = [500, 501, 502, 600, 700, 800, 801, 20, 21, 22]
    total_user_ids = [61, 150, 151, 152, 153, 351, 352, 353, 200]
    total_user_scores = []
    haptic_user_scores = []
    visual_user_scores = []

    for user_id in user_ids:
        user_scores = get_scores_for_user(user_id)
        haptic_user_scores.append(user_scores[0])
        visual_user_scores.append(user_scores[1])
    for user_id in total_user_ids:
        total_user_scores.append(get_scores_for_total_user(user_id))

    haptic_df = pl.DataFrame(haptic_user_scores, schema=[f"user_{user_id}" for user_id in user_ids])
    visual_df = pl.DataFrame(visual_user_scores, schema=[f"user_{user_id}" for user_id in user_ids])
    total_df = pl.DataFrame(total_user_scores, schema=[f"user_{user_id}" for user_id in total_user_ids])
    print(haptic_df.describe())
    print(visual_df.describe())
    print(total_df.describe())

    haptic_df.write_csv("haptic_df.csv")
    visual_df.write_csv("visual_df.csv")
    total_df.write_csv("total_df.csv")