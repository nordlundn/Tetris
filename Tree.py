import numpy as np
import MCTS
import Tetris


class Tree:
    def __init__(self, tetris, network):
        self.tetris = tetris
        self.network = network

        num_actions = self.tetris.get_num_actions(0)
        state = np.reshape(self.tetris.get_state(0), [1,-1])
        pis, vals = self.network.predict(state, [num_actions])
        policy = pis[0]
        value = vals[0]

        # policy = np.random.rand(num_actions)
        # value = np.random.rand();
        # print(type(value))

        # build tree
        self.tree = MCTS.MCTS()
        self.tree.make_root()
        self.tree.make_children(num_actions)
        A = self.tree.get_policy()
        A[:] = policy
        self.tree.set_params(value, 0)

    def step(self, tree, tetris):
        action = tree.step()

        if action == -1:
            prev_num_children = tetris.get_num_actions(1)
            states = tetris.explore()
            states = np.reshape(states, [-1, 19])
            game_over = (states[:,0].flatten()).tolist()
            num_children = (states[:,1].flatten()).tolist()
            states = states[:,2:]

            policies, values = self.network.predict(states, num_children)

            # policies = []
            # values = []
            #
            # for i in range(prev_num_children):
            #     policies.append(np.random.rand(int(num_children[i])))
            #     values.append(np.random.rand()-1)

            for i in range(prev_num_children):
                if int(num_children[i]) > 0:
                    tree.make_grandchildren(i, int(num_children[i]))
                    _policy = tree.get_child_policy(i)
                    _policy[:] = policies[i]
                tree.set_child_params(i, values[i], int(game_over[i]))

            tetris.reset_search()
            tree.reset()

        elif action == -2:
            tree.reset()
            tetris.reset_search()

        else:
            tetris.take_action(action, 1)
    def run(self, reps):
        for i in range(reps):
            # print(i)
            self.step(self.tree, self.tetris)
        return self.tree.get_pi(.8)

class Game:
    def __init__(self, network, game_length, search_size):
        self.network = network
        self.length = game_length
        self.search_size = search_size
        x = np.random.randint(7, size=self.length)
        self.tetris = Tetris.Tetris(x,self.length)
        self.experiences = []

    def step(self):
        tree = Tree(self.tetris, self.network)
        pi = tree.run(self.search_size)
        padded_pi = np.zeros(34)
        padded_pi[:pi.shape[0]] = pi
        state = self.tetris.get_state(0)
        self.experiences.append((state,padded_pi))
        action = np.random.choice(len(pi), p=pi)
        gameOver = self.tetris.take_action(action, 0)
        return gameOver
    def playOut(self, buffer):
        i = 0
        while not self.step():
            i += 1
            # print(i)

        reward = self.tetris.get_reward()
        reward = 1/(reward-(4*self.length-1))
        self.network.writeReward(reward)
        for experience in self.experiences:
            buffer.add(experience[0], experience[1], reward)
        self.experiences = []

    def show(self):
        self.tetris.print(0)
        reward = self.tetris.get_reward()
        print(reward, 1/(reward-(4*self.length-1)))

    def reset(self):
        self.tetris.reset_committed()

# game = Game(None, 10, 1000)
# game.playOut()
# game.show()

# tree = Tree(tetris, None)
# print(tree.run(1000))
