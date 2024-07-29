from sklearn.neighbors import NearestNeighbors
from sklearn.datasets import make_blobs
from sklearn.metrics import precision_recall_curve, roc_curve, auc
import numpy as np
import matplotlib.pyplot as plt

class LOF:
    def __init__(self, n_neighbors=10, metric='euclidean', contamination=0.01, njobs=1):
        self.n_neighbors = n_neighbors
        self.metric = metric
        self.njobs = njobs
        self.contamination = contamination
        self.neighbors = None

    def lrd(self, x):
        assert self.neighbors != None
        distance, neighbors = self.neighbors.kneighbors(x, self.n_neighbors + 1, self.metric)
        distance, neighbors = distance[:, 1:], neighbors[:, 1:]
        reach_distances = []
        for i, point in enumerate(x):
            neighbors_index = neighbors[i]
            neighbors_radius = self.radius[neighbors_index]
            reach_distance_i = np.concatenate(
                [neighbors_radius[distance[i] <= neighbors_radius], distance[i][distance[i] > neighbors_radius]],
                axis=0)
            reach_distances.append(reach_distance_i)
        reach_distances = np.stack(reach_distances)
        lrd_x = self.n_neighbors / reach_distances.sum(axis=1)
        return lrd_x

    def decision_function(self, x):
        lrd_x = self.lrd(x)
        distance, neighbors = self.neighbors.kneighbors(x, self.n_neighbors + 1, self.metric)
        neighbors = neighbors[:, 1:]
        lof = self.lrd_x_[neighbors].mean(axis=-1) / lrd_x
        return lof

    def fit(self, x):
        self.neighbors = NearestNeighbors(n_neighbors=self.n_neighbors, metric=self.metric).fit(x)
        distance, neighbors = self.neighbors.kneighbors(x, self.n_neighbors + 1, self.metric)
        distance, neighbors = distance[:, 1:], neighbors[:, 1:]
        self.radius = distance.max(axis=-1)  # k近邻领域半径
        self.lrd_x_ = self.lrd(x)  # 局部可达密度
        self.decision_scores_ = self.decision_function(x)
        self.threshold_ = np.percentile(self.decision_scores_, (1 - self.contamination) * 100)
        self.labels_ = (self.decision_scores_ >= self.threshold_).astype(int)
        return self

    def predict(self, x):
        lof = self.decision_function(x)
        labels = np.zeros(x.shape[0])
        labels[lof >= self.threshold_] = 1
        return labels

# 生成数据点，inlier为正常点，outlier为人造异常
    '''
    n_samples:生成数据个数
    n_features:生成数据维度
    centers:每一个簇的中心位置
    cluster_std:每一个簇内数据的生成标准差
    '''
inliers, class_labels = make_blobs(n_samples=1500, n_features=2, centers=[[1, 1], [5, 2], [3, 10]],
                                       cluster_std=[0.25, 0.25, 0.3])
outliers = np.array(
        [[2.2, 1.], [1.5, 2.5], [5., 4.], [0.5, 5.8], [5., 10.], [2., 7.8], [3., 6.], [3.1, 5.9], [2.9, 6.1], [3., 6.3],
         [3.1, 6.1]])
labels = np.concatenate([np.zeros(inliers.shape[0]), np.ones(outliers.shape[0])])
data = np.concatenate([inliers, outliers], axis=0)

plt.scatter(inliers[:, 0], inliers[:, 1], c='k', s=2, zorder=10, label='inlier')
plt.scatter(outliers[:, 0], outliers[:, 1], c='r', s=2, zorder=10, label='outlier')
plt.show()

    # 训练模型
lof = LOF(n_neighbors=20, metric='euclidean').fit(data)
anomaly_scores = lof.decision_scores_
labels = lof.labels_
threshold = lof.threshold_
# 模型评估
fpr, tpr, _ = roc_curve(labels, anomaly_scores, pos_label=1)
precision, recall, _ = precision_recall_curve(labels, anomaly_scores, pos_label=1)
auc_roc = auc(fpr, tpr)
auc_pr = auc(recall, precision)

# 绘制异常分等值图，橙色为决策边界
x, y = np.meshgrid(np.linspace(-5, 10), np.linspace(-5, 15))
scores = lof.decision_function(np.c_[x.ravel(), y.ravel()])
plt.scatter(inliers[:, 0], inliers[:, 1], c='k', s=2, zorder=10, label='inlier')
plt.scatter(outliers[:, 0], outliers[:, 1], c='r', s=2, zorder=10, label='outlier')
plt.contourf(x, y, scores.reshape(x.shape)*-1, cmap=plt.cm.Blues_r, levels=np.linspace(-1*scores.max(), -1*threshold, 10))
c = plt.contour(x, y, scores.reshape(x.shape), levels=np.linspace(threshold, scores.max(), 10), colors='k', linestyles='solid', alpha=0.6, linewidths=0.6)
plt.contourf(x, y, scores.reshape(x.shape)*-1, colors='orange', levels=[-1*threshold, -1*scores.min()])
plt.legend(loc='upper right')
plt.clabel(c, inline=True, fontsize=7)
plt.title(f'auc-roc={auc_roc:.2f}, auc-pr={auc_pr:.2f}')
plt.show()