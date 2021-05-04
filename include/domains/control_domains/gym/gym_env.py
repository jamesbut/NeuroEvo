import gym

def make_env(env_id):
    global env
    env = gym.make(env_id)

def reset():
    init_state = env.reset()
    return init_state.tolist()

def step(action, render):

    if render:
        env.render()
    s, r, done, info = env.step(action)

    print("Python s:", s)

    #Convert numpy array to list
    s = s.tolist()

    return s, r, done

def close():
    env.close()

'''
make_env('MountainCar-v0')
init_state = reset()
step_state = step(2, False)

print("Init state:", init_state)
print("Step state:", step_state)
'''
