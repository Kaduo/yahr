import polars as pl
import plotly.express as px
import plotly.graph_objects as go

def get_filename_of_tap(name, problem_id, haptics):
    haptics_folder = "haptics" if haptics else "no_haptics"
    return f"data/experiment/results/{haptics_folder}/{name}/problem_{problem_id}.tap"

def get_start_time_of_tap(name, problem_id, haptics):
    filename = get_filename_of_tap(name, problem_id, haptics)
    with open(filename, "r") as f:
        return int(f.readline())
    
def get_end_time_of_tap(name, problem_id, haptics):
    return get_start_time_of_tap(name, problem_id+1, haptics)

def crop_times_to_problem(name, problem_id, haptics):
    haptics_folder = "haptics" if haptics else "no_haptics"
    eye_tracking = pl.read_csv(f"data/experiment/results/{haptics_folder}/{name}/{name}.csv")
    eye_tracking = eye_tracking.with_columns((eye_tracking["Timestamp (ms)"]/1000).alias("Timestamp (s)"))
    start = get_start_time_of_tap(name, problem_id, haptics)
    end = get_end_time_of_tap(name, problem_id, haptics)
    print(end - start)
    df = eye_tracking.filter((eye_tracking["Timestamp (s)"] <= end) & (start <= eye_tracking["Timestamp (s)"]))
    df = df.with_columns((df["Timestamp (s)"] - df["Timestamp (s)"][0]).alias("Elapsed time (s)"))
    return df

def get_problem_duration(name, problem_id, haptics):
    return get_end_time_of_tap(name, problem_id, haptics) - get_start_time_of_tap(name, problem_id, haptics)

def get_touching_boundaries_from_tap(name, problem_id, haptics):
    res_list = []
    filename = get_filename_of_tap(name, problem_id, haptics)
    touching = False
    # absolute_start_time = get_start_time_of_tap(name, problem_id, haptics)
    latest_time = None
    relative_start_time = None
    with open(filename, "r") as f:
        for line in f:
            split_line = line.split()
            if split_line[0] == "leftMouseButtonFlipped":
                touching = not touching
                if touching:
                    res_list.append([latest_time])
                else:
                    res_list[-1].append(latest_time)
            if split_line[0] == "time":
                if latest_time is None:
                    latest_time = 0
                    relative_start_time = float(split_line[1])
                else:
                    latest_time = float(split_line[1]) - relative_start_time

    return res_list

# if __name__ == "__main__":
#     name = "louis"
#     problem_id = 4
#     fig = px.line(crop_times_to_problem(name, problem_id, True), x="Elapsed time (s)", y="Pitch")
#     print(crop_times_to_problem(name, problem_id, True))
#     fig.show()

if __name__ == "__main__":
    name = "jp"
    problem_id = 8
    haptics = True
    touching_bounds = get_touching_boundaries_from_tap(name, problem_id, haptics)
    fig = px.line(crop_times_to_problem(name, problem_id, haptics), x="Elapsed time (s)", y="Pitch")
    for bounds in touching_bounds:
        fig.add_vrect(bounds[0], bounds[1], opacity=0.5, fillcolor="LightSalmon", line_width=0, layer="below")
    # fig = px.
    fig.show()