#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Граф переходов для Системы 2 (Гиперэкспоненциальная)
"""

import numpy as np
import math

print("=" * 100)
print("  ГРАФ ПЕРЕХОДОВ СИСТЕМЫ 2 - ГИПЕРЭКСПОНЕНЦИАЛЬНАЯ M/H₂.₂/1/2")
print("=" * 100)
print()

# Параметры
LAMBDA = 0.7
B = 10.0
V = 2.2
q = 1.0 / 7.0

# Расчет μ₁ и μ₂
term1 = math.sqrt((1 - q) / (2 * q) * (V**2 - 1))
b1_prime = (1 + term1) * B
mu1 = 1.0 / b1_prime

term2 = math.sqrt(q / (2 * (1 - q)) * (V**2 - 1))
b2_prime = (1 - term2) * B
mu2 = 1.0 / b2_prime

lambda1 = q * LAMBDA
lambda2 = (1 - q) * LAMBDA

print("ПАРАМЕТРЫ СИСТЕМЫ:")
print("-" * 100)
print(f"  λ = {LAMBDA} заявок/сек")
print(f"  q = {q:.4f}")
print(f"  λ₁ = q × λ = {lambda1:.4f} (поток на фазу 1)")
print(f"  λ₂ = (1-q) × λ = {lambda2:.4f} (поток на фазу 2)")
print(f"  μ₁ = {mu1:.6f} (интенсивность фазы 1)")
print(f"  μ₂ = {mu2:.6f} (интенсивность фазы 2)")
print()

# Состояния
states = [
    ("S0", "(0,-)", "Система пуста"),
    ("S1", "(1,1)", "1 заявка на обслуживании, фаза 1"),
    ("S2", "(1,2)", "1 заявка на обслуживании, фаза 2"),
    ("S3", "(2,1)", "2 заявки (1 в очереди), фаза 1"),
    ("S4", "(2,2)", "2 заявки (1 в очереди), фаза 2"),
    ("S5", "(3,1)", "3 заявки (очередь полна), фаза 1"),
    ("S6", "(3,2)", "3 заявки (очередь полна), фаза 2"),
]

print("СОСТОЯНИЯ СИСТЕМЫ (N, φ):")
print("-" * 100)
print("N - количество заявок в системе (включая обслуживаемую)")
print("φ - фаза обслуживания (1 или 2)")
print()
for name, notation, desc in states:
    print(f"  {name}: {notation:6} - {desc}")
print()

# Строим матрицу переходов
Q = np.zeros((7, 7))

# S0 -> S1, S2
Q[0, 1] = lambda1
Q[0, 2] = lambda2

# S1
Q[1, 0] = mu1
Q[1, 3] = lambda1
Q[1, 4] = lambda2

# S2
Q[2, 0] = mu2
Q[2, 3] = lambda1
Q[2, 4] = lambda2

# S3
Q[3, 1] = mu1 * q
Q[3, 2] = mu1 * (1 - q)
Q[3, 5] = lambda1
Q[3, 6] = lambda2

# S4
Q[4, 1] = mu2 * q
Q[4, 2] = mu2 * (1 - q)
Q[4, 5] = lambda1
Q[4, 6] = lambda2

# S5
Q[5, 3] = mu1 * q
Q[5, 4] = mu1 * (1 - q)

# S6
Q[6, 3] = mu2 * q
Q[6, 4] = mu2 * (1 - q)

# Диагональ
for i in range(7):
    Q[i, i] = -np.sum(Q[i, :])

# Матрица смежности
A = (Q != 0).astype(int)
for i in range(7):
    A[i, i] = 0

print("=" * 100)
print("  МАТРИЦА СМЕЖНОСТИ (1 = есть переход, 0 = нет перехода)")
print("=" * 100)
print()
print("      ", end="")
for j in range(7):
    print(f" S{j} ", end="")
print()
print("-" * 100)

for i in range(7):
    print(f"  S{i}  ", end="")
    for j in range(7):
        if A[i, j] == 1:
            print("  ●  ", end="")
        else:
            print("  ·  ", end="")
    print()
print()

# Список всех переходов
print("=" * 100)
print("  ВСЕ ПЕРЕХОДЫ С ИНТЕНСИВНОСТЯМИ")
print("=" * 100)
print()

transitions = []
for i in range(7):
    for j in range(7):
        if i != j and Q[i, j] > 0:
            transitions.append((i, j, Q[i, j]))

for state_from in range(7):
    trans_from = [(j, rate) for i, j, rate in transitions if i == state_from]
    if trans_from:
        print(f"ИЗ {states[state_from][0]} {states[state_from][1]} → ")
        for state_to, rate in trans_from:
            # Определяем тип перехода
            if state_from == 0:
                if state_to == 1:
                    trans_type = f"λ₁={lambda1:.4f}"
                else:
                    trans_type = f"λ₂={lambda2:.4f}"
            elif state_to < state_from or (state_from >= 5 and state_to == 3) or (state_from >= 5 and state_to == 4):
                # Завершение обслуживания
                if state_from in [1, 3, 5]:
                    if state_to in [0, 1, 2, 3, 4]:
                        trans_type = f"μ₁={mu1:.4f}" if state_to in [0] else f"μ₁×q={mu1*q:.4f}" if state_to == 1 else f"μ₁×(1-q)={mu1*(1-q):.4f}"
                else:
                    trans_type = f"μ₂={mu2:.4f}" if state_to == 0 else f"μ₂×q={mu2*q:.4f}" if state_to == 1 else f"μ₂×(1-q)={mu2*(1-q):.4f}"
            else:
                # Прибытие
                if state_to == state_from + 2 or (state_from <= 2 and state_to == 3):
                    trans_type = f"λ₁={lambda1:.4f}"
                else:
                    trans_type = f"λ₂={lambda2:.4f}"
            
            print(f"   → {states[state_to][0]} {states[state_to][1]}: {rate:.6f}  ({trans_type})")
        print()

# ASCII визуализация
print("=" * 100)
print("  ГРАФ ПЕРЕХОДОВ (ВИЗУАЛИЗАЦИЯ)")
print("=" * 100)
print()

print("""
                    СИСТЕМА 2: ГИПЕРЭКСПОНЕНЦИАЛЬНАЯ M/H₂.₂/1/2
                    
                    1 прибор, 2 фазы обслуживания
                    Очередь до 2 заявок
                    
                                  S0
                              (0, пусто)
                                  │
                    ┌─────────────┴─────────────┐
                    │                           │
             λ₁=0.1 │                           │ λ₂=0.6
             (фаза 1)                    (фаза 2)
                    ↓                           ↓
              ┌──────────┐                ┌──────────┐
              │    S1    │                │    S2    │
              │  (1, φ1) │                │  (1, φ2) │
              │ [1 заявка│                │ [1 заявка│
              │  фаза 1] │                │  фаза 2] │
              └─────┬────┘                └─────┬────┘
                    │                           │
             μ₁=0.023│                    μ₂=0.230│
                    ↓                           ↓
              ┌─────┴─────┐            ┌────────┴─────┐
              │  к S0     │            │   к S0       │
              └───────────┘            └──────────────┘
                    │                           │
        ┌───────────┼───────────┐   ┌───────────┼───────────┐
        │           │           │   │           │           │
   λ₁   │      λ₂   │           │   │      λ₁   │      λ₂   │
        ↓           ↓           │   ↓           ↓           │
   ┌─────────┐ ┌─────────┐     │ ┌─────────┐ ┌─────────┐   │
   │   S3    │ │   S4    │◄────┘ │   S3    │ │   S4    │◄──┘
   │ (2, φ1) │ │ (2, φ2) │       │ (2, φ1) │ │ (2, φ2) │
   │[q=1,φ1] │ │[q=1,φ2] │       │[q=1,φ1] │ │[q=1,φ2] │
   └────┬────┘ └────┬────┘       └────┬────┘ └────┬────┘
        │           │                 │           │
   μ₁×q │  μ₁×(1-q) │          μ₂×q  │  μ₂×(1-q) │
        ↓           ↓                 ↓           ↓
   ┌────┴──┐   ┌────┴──┐        ┌────┴──┐   ┌────┴──┐
   │  к S1 │   │  к S2 │        │  к S1 │   │  к S2 │
   └───────┘   └───────┘        └───────┘   └───────┘
        │           │                 │           │
   λ₁   │      λ₂   │            λ₁   │      λ₂   │
        ↓           ↓                 ↓           ↓
   ┌─────────┐ ┌─────────┐      ┌─────────┐ ┌─────────┐
   │   S5    │ │   S6    │      │   S5    │ │   S6    │
   │ (3, φ1) │ │ (3, φ2) │      │ (3, φ1) │ │ (3, φ2) │
   │[q=2,φ1] │ │[q=2,φ2] │      │[q=2,φ1] │ │[q=2,φ2] │
   │ ПОЛНАЯ  │ │ ПОЛНАЯ  │      │ ПОЛНАЯ  │ │ ПОЛНАЯ  │
   └────┬────┘ └────┬────┘      └────┬────┘ └────┬────┘
        │           │                │           │
   μ₁×q │  μ₁×(1-q) │         μ₂×q  │  μ₂×(1-q) │
        ↓           ↓                ↓           ↓
   ┌────┴──┐   ┌────┴──┐       ┌────┴──┐   ┌────┴──┐
   │  к S3 │   │  к S4 │       │  к S3 │   │  к S4 │
   └───────┘   └───────┘       └───────┘   └───────┘
        
        
ОБОЗНАЧЕНИЯ:
  λ₁ = 0.1000   - прибытие заявки → начало фазы 1
  λ₂ = 0.6000   - прибытие заявки → начало фазы 2
  μ₁ = 0.0228   - завершение фазы 1
  μ₂ = 0.2302   - завершение фазы 2
  q  = 0.1429   - вероятность выбора фазы 1
  
ОСОБЕННОСТИ:
  • Прибывающая заявка начинает обслуживание в фазе 1 с вероятностью q
  • Прибывающая заявка начинает обслуживание в фазе 2 с вероятностью (1-q)
  • После завершения фазы заявка уходит из системы
  • Следующая заявка из очереди начинает фазу 1 с вероятностью q
  • Из состояний S5 и S6 (полная очередь) новые заявки теряются
""")

# Матрица интенсивностей
print()
print("=" * 100)
print("  МАТРИЦА ИНТЕНСИВНОСТЕЙ ПЕРЕХОДОВ Q")
print("=" * 100)
print()
print("       ", end="")
for j in range(7):
    print(f"    S{j}    ", end="")
print()
print("-" * 100)

for i in range(7):
    print(f"  S{i}  ", end="")
    for j in range(7):
        if Q[i, j] != 0:
            print(f" {Q[i, j]:8.5f}", end="")
        else:
            print(f"    ·    ", end="")
    print()
print()

# Graphviz код
print("=" * 100)
print("  КОД ДЛЯ GRAPHVIZ (сохраните как graph2.dot)")
print("=" * 100)
print()

graphviz_code = f"""digraph System2 {{
    rankdir=TB;
    node [shape=circle, style=filled, fillcolor=lightgreen, fontsize=10];
    edge [fontsize=9];
    
    label="Граф переходов Системы 2 (M/H₂.₂/1/2)\\nλ₁={lambda1:.3f}, λ₂={lambda2:.3f}, μ₁={mu1:.4f}, μ₂={mu2:.4f}";
    labelloc="t";
    fontsize=14;
    
    // Состояния
    S0 [label="S0\\n(0,-)\\n(пусто)", fillcolor=lightblue];
    S1 [label="S1\\n(1,φ1)\\n(фаза 1)", fillcolor=lightgreen];
    S2 [label="S2\\n(1,φ2)\\n(фаза 2)", fillcolor=lightyellow];
    S3 [label="S3\\n(2,φ1)\\n(q=1,φ1)", fillcolor=lightgreen];
    S4 [label="S4\\n(2,φ2)\\n(q=1,φ2)", fillcolor=lightyellow];
    S5 [label="S5\\n(3,φ1)\\n(ПОЛНА)", fillcolor=pink];
    S6 [label="S6\\n(3,φ2)\\n(ПОЛНА)", fillcolor=pink];
    
    // Переходы - группируем по типам
    
    // Прибытия с началом фазы 1 (синие)
    S0 -> S1 [label="λ₁={lambda1:.3f}", color=blue];
    S1 -> S3 [label="λ₁={lambda1:.3f}", color=blue];
    S2 -> S3 [label="λ₁={lambda1:.3f}", color=blue];
    S3 -> S5 [label="λ₁={lambda1:.3f}", color=blue];
    S4 -> S5 [label="λ₁={lambda1:.3f}", color=blue];
    
    // Прибытия с началом фазы 2 (зеленые)
    S0 -> S2 [label="λ₂={lambda2:.3f}", color=green];
    S1 -> S4 [label="λ₂={lambda2:.3f}", color=green];
    S2 -> S4 [label="λ₂={lambda2:.3f}", color=green];
    S3 -> S6 [label="λ₂={lambda2:.3f}", color=green];
    S4 -> S6 [label="λ₂={lambda2:.3f}", color=green];
    
    // Завершения фазы 1 (красные)
    S1 -> S0 [label="μ₁={mu1:.4f}", color=red];
    S3 -> S1 [label="μ₁×q={mu1*q:.4f}", color=red, style=dashed];
    S3 -> S2 [label="μ₁×(1-q)={mu1*(1-q):.4f}", color=red, style=dashed];
    S5 -> S3 [label="μ₁×q={mu1*q:.4f}", color=red, style=dashed];
    S5 -> S4 [label="μ₁×(1-q)={mu1*(1-q):.4f}", color=red, style=dashed];
    
    // Завершения фазы 2 (оранжевые)
    S2 -> S0 [label="μ₂={mu2:.4f}", color=orange];
    S4 -> S1 [label="μ₂×q={mu2*q:.4f}", color=orange, style=dashed];
    S4 -> S2 [label="μ₂×(1-q)={mu2*(1-q):.4f}", color=orange, style=dashed];
    S6 -> S3 [label="μ₂×q={mu2*q:.4f}", color=orange, style=dashed];
    S6 -> S4 [label="μ₂×(1-q)={mu2*(1-q):.4f}", color=orange, style=dashed];
}}
"""

print(graphviz_code)

# Сохраняем в файл
with open('/workspace/graph2.dot', 'w') as f:
    f.write(graphviz_code)

print()
print("=" * 100)
print("  ✅ Файл graph2.dot создан!")
print()
print("  ДЛЯ ВИЗУАЛИЗАЦИИ:")
print("  1. Откройте: https://dreampuf.github.io/GraphvizOnline/")
print("  2. Скопируйте код выше или содержимое файла graph2.dot")
print("  3. Вставьте на сайт")
print("=" * 100)
print()
