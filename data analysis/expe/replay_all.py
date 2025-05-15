import subprocess
import time

# if __name__ == "__main__":
#     haptic_names = ["ecma1", "ecma2", "hugo", "jp", "louis"]
#     for name in haptic_names:
#         for i in range(5, 15):
#             print(name, i)
#             subprocess.run(["build/expe_replay", f"data/experiment/results/haptics/{name}/problem_{i}.tap",
#                             f"data/experiment/results/haptics/{name}/problem_{i}_start.rods"])
#             time.sleep(5)

if __name__ == "__main__":
    haptic_names = ["ecma3", "emma", "enzo", "mathis", "romain"]
    for name in haptic_names:
        for i in range(5, 15):
            print(name, i)
            subprocess.run(["build/expe_replay", f"data/experiment/results/no_haptics/{name}/problem_{i}.tap",
                            f"data/experiment/results/no_haptics/{name}/problem_{i}_start.rods"])
            time.sleep(5)
