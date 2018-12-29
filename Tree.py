import numpy as np
import MCTS
import Tetris

# x = np.random.randint(7, size=10);
# x = np.array([3,0,1,1,1,1,1,1,1,1])
x = np.array([1,0,3,3,3,3,3,3,3,3])
print(x)
tetris = Tetris.Tetris(x, 10)
num_actions = tetris.get_num_actions(0)

# num_moves = np.array([9,17,34])
# x = np.random.randint(3, size=10)
# policy = np.random.rand(num_moves[x[0]])
# value = np.random.rand();
# move_num = 0

policy = np.random.rand(num_actions)
value = np.random.rand();

# root parameters
tree = MCTS.MCTS()
tree.make_root()
# tree.make_children(int(num_moves[x[0]]))
tree.make_children(num_actions)
A = tree.get_policy()
A[:] = policy
tree.set_params(value, 0)
# assert 0 == 1
# tree.print_policy()

def step(tree, tetris):
    a = len(tree.get_policy())
    b = tetris.get_num_actions(1)
    # print(a, b)
    action = tree.step()
    # print(action)
    if action > -2:
        assert a == b
    # print(action)
    # tree.print_policy()

    if action == -1:
        print("Unvisited")
        prev_num_children = tetris.get_num_actions(1)
        # print("Current unvisited node number of children ", prev_num_children)
        states = tetris.explore()
        # print("Explored")
        states = np.reshape(states, [-1, 19])
        game_over = (states[:,0].flatten()).tolist()
        num_children = (states[:,1].flatten()).tolist()
        # print("Number of grandchildren ", num_children[0])
        # states = states[:,2:]
        policies = []
        values = []
        # assert len(game_over) == prev_num_children
        for i in range(prev_num_children):
            if int(game_over[i]) == 1:
                # print("Game over children is ", num_children[i])
                num_children[i] = 1
            policies.append(np.random.rand(int(num_children[i])))
            values.append(np.random.rand())

        # move_num += 1 # explore
        # if move_num >= 10:
        #     game_over = 1
        #     num_children = 1
        # else:
        #     game_over = 0
        #     num_children = num_moves[x[move_num]]
        # prev_num_children  = num_moves[x[move_num-1]]
        # policies = [np.random.rand(int(num_children)) for i in range(prev_num_children)]
        # values = [np.random.rand() for i in range(prev_num_children)]

        for i in range(prev_num_children):
            tree.make_grandchildren(i, int(num_children[i]))
            _policy = tree.get_child_policy(i)
            _policy[:] = policies[i]
            tree.set_child_params(i, values[i], int(game_over[i]))
        # print("Sanity check ", len(tree.get_policy()), len(tree.get_child_policy(0)))
        tetris.reset_search()
        tree.reset()
        # move_num = 0
        # return move_num
    elif action == -2:
        print("game over")
        tree.reset()
        tetris.reset_search()
        # move_num = 0
        # return move_num
    else:
        print("taking action")
        tetris.take_action(action, 1)
        # move_num+=1
        # return move_num

for i in range(10000):
    # move_num = step(tree,tetris)
    step(tree, tetris)
    # print(i, move_num)
    print(i)
print("finished, now just have to free c++ memory")

# def step(tree, path_length):
#     action_idx = tree.step()
#     # print(action_idx)
#     if action_idx == -1:
#         policies = [np.random.rand(4) for i in range(4)]
#         values = [np.random.rand() for i in range(4)]
#         if path_length == num_moves:
#             game_over = [1]*4
#         else:
#             game_over = [0]*4
#         print(game_over)
#         num_children = [4]*4
#         tree.set_values(policies, values, game_over, num_children)
#         return False, 0
#     if action_idx == -2:
#         tree.set_values([],[],[],[])
#         return False, 0
#     else:
#         return True, path_length+1
#
# path_length = 0
# for i in range(10000):
#     ongoing, path_length = step(tree,path_length)
#     assert path_length <= num_moves+1
#     print(ongoing, path_length, i)
