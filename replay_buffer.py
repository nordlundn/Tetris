from collections import deque

import random
import numpy as np

class ReplayBuffer(object):
    def __init__(self, buffer_size, random_seed=123):
        self.buffer_size = buffer_size
        self.count = 0
        self.buffer = deque()
        random.seed(random_seed)

    def add(self, state, policy, value):
        experience = (state, policy, value)
        if self.count < self.buffer_size:
            self.buffer.append(experience)
            self.count += 1
        else:
            self.buffer.popleft()
            self.buffer.append(experience)

    def size(self):
        return self.count

    def sample_batch(self, batch_size):
        batch = []
        if self.count < batch_size:
            batch = random.sample(self.buffer, self.count)
        else:
            batch = random.sample(self.buffer, batch_size)

        state_batch = np.array([_[0] for _ in batch])
        policy_batch = np.array([_[1] for _ in batch])
        value_batch = np.reshape(np.array([_[2] for _ in batch]), [min(batch_size, self.count), 1])

        return state_batch, policy_batch, value_batch

    def clear(self):
        self.buffer.clear()
        self.count = 0
