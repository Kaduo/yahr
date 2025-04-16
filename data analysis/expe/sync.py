import polars as pl
import plotly.express as px

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

# if __name__ == "__main__":
#     name = "louis"
#     problem_id = 4
#     fig = px.line(crop_times_to_problem(name, problem_id, True), x="Elapsed time (s)", y="Pitch")
#     print(crop_times_to_problem(name, problem_id, True))
#     fig.show()

def get_problem_duration(name, problem_id, haptics):
    return get_end_time_of_tap(name, problem_id, haptics) - get_start_time_of_tap(name, problem_id, haptics)

if __name__ == "__main__":
    name = "romain"
    for i in range(10):
        print(get_problem_duration(name, i, False))
