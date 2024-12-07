# -*- coding: utf-8 -*-
"""
Created on Tue Jun 27 17:11:38 2023

@author: dokli
"""

from typing import List
import numpy as np
import random
import matplotlib.pyplot as plt
import re
from glob import glob

class Node:
    def __init__(self, coord : np.ndarray, childs : List['Node'] = []) -> None:
        self.coord = coord
        self.childs = childs

    def __str__(self) -> str:
        ret = f"current : {self.coord}\nchilds : "
        if not self.childs:
            ret += "None"
        for child in self.childs:
            ret += f"{child}"
        return ret + "\n"
    

def my_thunder(start: np.ndarray, end: np.ndarray, nbMidPoint: int, depth: int, maxDepth: int) -> List[np.ndarray]:
    if depth == maxDepth:
        return [start, end]

    vec = (end - start) / (nbMidPoint + 1)
    
    pos = []
    
    #for i in range(nbMidPoint + 2) :
    #    val = start + i * vec
        
    #    if i != 0 and i != nbMidPoint + 1 :
    #        new_val = [(random.random() * 2 - 1) * vec[0] * (maxDepth / (np.sqrt(depth) + 1)), 
    #                   (random.random() - 0.2) * vec[1]]
    #        val += np.array(new_val)
    #        pos.append(val)
            
    #    pos.append(val)

    pos = [start + i * vec + (0 if i == 0 or i == nbMidPoint + 1 else (np.array([(random.random() * 2 - 1) * vec[0] * (
        maxDepth / (np.sqrt(depth) + 1)), (random.random() - 0.2) * vec[1]]))) for i in range(nbMidPoint + 2)]

    ret = []
    for i in range(len(pos) - 1) :
        ret += my_thunder(pos[i], pos[i + 1], nbMidPoint, depth + 1, maxDepth)

    return ret


def my_thunder_tree(start: np.ndarray, end: np.ndarray, nbMidPoint: int, depth: int, maxDepth: int) -> List[np.ndarray]:
    if depth == maxDepth:
        return (Node(start, childs=[Node(end, childs=[])]), 2)

    vec = (end-start) / (nbMidPoint+1)

    pos = [start + i * vec + (0 if i == 0 or i == nbMidPoint + 1 else (np.array([(random.random() * 2 - 1) * vec[0] * (maxDepth / (np.sqrt(depth) + 1)), (random.random()-0.2) * vec[1]]))) for i in range(nbMidPoint + 2)]

    ret = None
    size = 0
    for i in range(len(pos) - 1):
        (chunk, curr_size) = my_thunder_tree(pos[i], pos[i+1], nbMidPoint, depth+1, maxDepth)
        size += curr_size
        
        if ret is None:
            ret = chunk
        
        else:
            tmp_ret : Node = ret
            
            while tmp_ret.childs:
                tmp_ret = tmp_ret.childs[0]
                
            tmp_ret.childs.append(chunk)

    return (ret, size)


def my_thunder_trees(root: Node, start: np.ndarray, end: np.ndarray, max_height: int, height: int) :
    if height >= max_height or len(root.childs) == 0 :
        return
    
    #normalized_height = (height / max_height) * 5
    
    #eps = random.uniform(normalized_height, 5.0)
    
    #p1 = np.exp(-(normalized_height + eps))
    
    p1 = random.uniform(0., 1.) * (1 - (height / max_height))

    if p1 >= 0.8 :
        vec = end - start;
        
        adj = np.sqrt((vec[0] ** 2) + (vec[1] ** 2))
        
        angle = 3.14 / random.uniform(10., 15.)
        
        hyp = adj / np.cos(angle)
        
        opp = np.sin(angle) * hyp
        
        p2 = random.uniform(0., 1.)
       
        new_x = end[0]
        new_y = start[1] * random.uniform(0.1, 0.5)
        
        if (p2 >= 0.5) :
            new_x -= opp
            
        else :
            new_x += opp
        
        new_start = root.coord
        
        new_end = np.array([new_x, new_y])
        
        child_root, child_size = my_thunder_tree(new_start, new_end, 4, 0, 3)
        
        if len(child_root.childs) > 0 :
            root.childs.append(child_root.childs[0])
    
    my_thunder_trees(root.childs[0], root.childs[0].coord, end, max_height, height + 1)
    return
    

def thunder_frames_distance(root: Node, max_height: int, frame_size_percentage: float) :
    new_root = root # Racine du nouvel arbre à retourner
    
    queue = [new_root]
    
    height = 0
    height_limit = int(max_height * frame_size_percentage)
    
    while len(queue) > 0 and height < height_limit :
        currentNode = queue.pop(0)
        
        if currentNode == None :
            height += 1
            
        else :
        
            i = 0
            while i < len(currentNode.childs) :
            
                if (height == height_limit - 1) :
                    finalNode = currentNode.childs[i]
                    finalNode.childs = [] # Derniers noeuds du tronçon, donc on enlève leurs enfants
                
                    queue.append(finalNode)
                    
                else :
                    queue.append(currentNode.childs[i])
            
                i += 1
        
            if len(queue) > 0 :
                queue.append(None) # Sépérateur pour la hauteur
            
    return new_root

def thunder_frames_distance_list(root: Node, max_height: int, frame_size_percentage: float) :
    res = [] # Liste des données à retourner
    
    currentNode = root
    
    queue = [currentNode]
    
    height = 0
    height_limit = int(max_height * frame_size_percentage)
    
    while len(queue) > 0 and height < height_limit :
        currentNode = queue.pop(0)
        
        if currentNode == None :
            height += 1
            
        else :
            
            res.append(currentNode.coord)
        
            i = 0
            while i < len(currentNode.childs) :
            
                if (height == height_limit - 1) :
                    finalNode = currentNode.childs[i]
                    finalNode.childs = [] # Derniers noeuds du tronçon, donc on enlève leurs enfants
                
                    queue.append(finalNode)
                    
                else :
                    queue.append(currentNode.childs[i])
            
                i += 1
        
            if len(queue) > 0 :
                queue.append(None) # Sépérateur pour la hauteur
            
    return res
        

def convert_to_list(root) :
    res = [root.coord]
    for i in range(len(root.childs)) :
        res = res + convert_to_list(root.childs[i])
    return res

begin = np.array([-20, 200])
end = np.array([20, 0])
NB = 1

plt.figure(figsize=(7, 7))
for i in range(NB):
    root, size = my_thunder_tree(begin, end, 4, 0, 3)
    print("lol")
    my_thunder_trees(root, begin, end, size - 1, 0)
    print("mdr")
    
    res = []
    
    #res = thunder_frames_distance_list(root, size - 1, 0.5)

    #print(max_height)

    res = convert_to_list(root)
    print("ptdr")
    
    res = np.array(res)

    if i == 0:
        print(len(res), size * 0.5)

    plt.plot(res[..., 0], res[..., 1], 'o')

    plt.ylim(-1, begin[1] + 1)
    plt.xlim(- begin[1]//2 - 1, begin[1]//2 + 1)

#plt.show()

#lst = glob("tests/*")
#plt.figure(figsize=(7, 7))

#for path in lst:
#    cppprint = None

#    with open(path, 'r') as f:
#        cppprint = "".join(f.readlines())


#    cppprintParsed = np.array([eval(re.findall(r"\(.+\)", i)[0]) for i in cppprint.split("\n") if "None" not in i and i.strip() != ""])

#    plt.plot(cppprintParsed[..., 0], cppprintParsed[..., 1])

#    plt.ylim(-1, cppprintParsed[0][1] + 1)
#    plt.xlim(- cppprintParsed[0][1]//2 - 1, cppprintParsed[0][1]//2 + 1)
    
    
#    print(path)

#print(cppprintParsed.shape)
#plt.show()

#begin = np.array([-20, 200])
#end = np.array([20, 0])
#NB = 10

#plt.figure(figsize=(7, 7))
#for i in range(NB):
#    res = np.array(my_thunder(begin, end, 5, 0, 3))

#    if i == 0:
#        print(len(res))

#    plt.plot(res[..., 0], res[..., 1])

#    plt.ylim(-1, begin[1] + 1)
#    plt.xlim(- begin[1]//2 - 1, begin[1]//2 + 1)

#plt.show()