import gym

def make_env(env_id):
    global env
    env = gym.make(env_id)

def reset():
    init_state = env.reset()
    return init_state.tolist()

def step(action, render):

    print("Python action:", action)
    print("Python render:", render)

    if render:
        env.render()
    s, r, done, info = env.step(action)

    #Convert numpy array to list
    s = s.tolist()

    print("Python s:", s)
    print("Python r:", r)
    print("Python done:", done)

    return s, r, done

def close():
    env.close()

def num_actions():
    return env.action_space.n

def state_size():
    return env.observation_space.shape[0]

'''
make_env('MountainCar-v0')
#init_state = reset()
#step_state = step(2, False)

#print("Init state:", init_state)
#print("Step state:", step_state)

print("Observation space:", observation_space())
print("Obs shape:", state_size())
print("Action space:", action_space())
print("Num actions:", num_actions())
'''
