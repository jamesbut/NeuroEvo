import gym
from gym.spaces import Discrete, Box

def make_env(env_id, **kwargs):
    global env
    env = gym.make(env_id, **kwargs)

def reset():
    init_state = env.reset()
    return init_state.tolist()

def step(action, render):

    if render:
        env.render()
    s, r, done, info = env.step(action)

    #Convert numpy array to list
    s = s.tolist()

    '''
    print("Python action:", action)
    print("Python render:", render)
    print("Python s:", s)
    print("Python r:", r)
    print("Python info:", info)
    print("Python done:", done)
    '''

    return s, r, done

def close():
    env.close()

def action_space():
    if isinstance(env.action_space, Discrete):
        return env.action_space.n
    if isinstance(env.action_space, Box):
        return list(env.action_space.shape), env.action_space.low, env.action_space.high

def state_size():
    return env.observation_space.shape[0]

def seed(seed_val):
    env.seed(seed_val)

'''
make_env('CartPole-v0', gravity=8.7)
init_state = reset()
step_state = step(1, False)

print("Init state:", init_state)
print("Step state:", step_state)
'''
