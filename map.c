#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <limits.h>
#include <conio.h>

#define INITIAL_BUFFER_SIZE 1024

int **matrix;
char **unique;
int unique_count;

char *
process_input(FILE *file)
{
    char *processed = NULL;
    int size = INITIAL_BUFFER_SIZE;
    int len = 0;
    int prev_space = 1; // 避免开头添加空格
    processed = malloc(size);
    if (!processed)
    {
        perror("malloc");
        exit(1);
    }

    int c;
    while ((c = fgetc(file)) != EOF)
    {
        if (isalpha(c))
        {
            if (len + 1 >= size)
            {
                size *= 2;
                processed = realloc(processed, size);
                if (!processed)
                {
                    perror("realloc");
                    exit(1);
                }
            }
            processed[len++] = tolower(c);
            prev_space = 0;
        }
        else
        {
            if (!prev_space)
            {
                if (len + 1 >= size)
                {
                    size *= 2;
                    processed = realloc(processed, size);
                    if (!processed)
                    {
                        perror("realloc");
                        exit(1);
                    }
                }
                processed[len++] = ' ';
                prev_space = 1;
            }
        }
    }
    processed[len] = '\0';
    return processed;
}

char **split_into_words(char *processed, int *word_count)
{
    char **words = NULL;
    *word_count = 0;
    char *token = strtok(processed, " ");
    while (token != NULL)
    {
        words = realloc(words, (*word_count + 1) * sizeof(char *));
        words[*word_count] = strdup(token);
        (*word_count)++;
        token = strtok(NULL, " ");
    }
    return words;
}

int compare_strings(const void *a, const void *b)
{
    return strcmp(*(const char **)a, *(const char **)b);
}

char **get_unique_words(char **words, int word_count, int *unique_count)
{
    char **unique = NULL;
    *unique_count = 0;
    for (int i = 0; i < word_count; i++)
    {
        int found = 0;
        for (int j = 0; j < *unique_count; j++)
        {
            if (strcmp(words[i], unique[j]) == 0)
            {
                found = 1;
                break;
            }
        }
        if (!found)
        {
            unique = realloc(unique, (*unique_count + 1) * sizeof(char *));
            unique[*unique_count] = strdup(words[i]);
            (*unique_count)++;
        }
    }
    qsort(unique, *unique_count, sizeof(char *), compare_strings);
    return unique;
}

int find_index(const char *word)
{
    int left = 0;
    int right = unique_count - 1;
    while (left <= right)
    {
        int mid = left + (right - left) / 2;
        int cmp = strcmp(word, unique[mid]);
        if (cmp == 0)
            return mid;
        else if (cmp < 0)
            right = mid - 1;
        else
            left = mid + 1;
    }
    return -1;
}

void showDirectedGraph()
{
    int size = unique_count;
    printf("Directed Graph:\n");
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (matrix[i][j] > 0)
                printf("(%s, %s, %d)\n", unique[i], unique[j], matrix[i][j]);
        }
    }
    printf("\n");
}

void queryBridgeWords(const char *word1, const char *word2)
{
    // 将输入单词转换为小写以匹配图中的单词
    char *lower_word1 = strdup(word1);
    for (int i = 0; lower_word1[i]; i++)
        lower_word1[i] = tolower(lower_word1[i]);
    char *lower_word2 = strdup(word2);
    for (int i = 0; lower_word2[i]; i++)
        lower_word2[i] = tolower(lower_word2[i]);

    // 查找单词在图中的索引
    int word1_idx = find_index(lower_word1);
    int word2_idx = find_index(lower_word2);

    free(lower_word1);
    free(lower_word2);

    // 检查单词是否存在
    if (word1_idx == -1 || word2_idx == -1)
    {
        printf("No %s or %s in the graph!\n", word1, word2);
        return;
    }

    // 收集所有桥接词
    char **bridges = NULL;
    int num_bridges = 0;

    for (int i = 0; i < unique_count; i++)
    {
        if (matrix[word1_idx][i] > 0 && matrix[i][word2_idx] > 0)
        {
            bridges = realloc(bridges, (num_bridges + 1) * sizeof(char *));
            bridges[num_bridges] = strdup(unique[i]);
            num_bridges++;
        }
    }

    // 输出结果
    if (num_bridges == 0)
        printf("No bridge words from %s to %s!\n", word1, word2);
    else
    {
        printf("The bridge words from %s to %s are: ", word1, word2);
        if (num_bridges == 1)
        {
            printf("%s", bridges[0]);
        }
        else
        {
            printf("%s", bridges[0]);
            for (int i = 1; i < num_bridges - 1; i++)
                printf(", %s", bridges[i]);
            printf(" and %s", bridges[num_bridges - 1]);
        }
        printf(".\n");
    }

    // 释放桥接词内存
    for (int i = 0; i < num_bridges; i++)
        free(bridges[i]);
    free(bridges);
}

char *process_string(const char *input)
{
    char *processed = NULL;
    int size = INITIAL_BUFFER_SIZE;
    int len = 0;
    int prev_space = 1;
    processed = malloc(size);
    if (!processed)
    {
        perror("malloc");
        exit(1);
    }

    for (int i = 0; input[i] != '\0'; i++)
    {
        int c = input[i];
        if (isalpha(c))
        {
            if (len + 1 >= size)
            {
                size *= 2;
                char *temp = realloc(processed, size);
                if (!temp)
                {
                    perror("realloc");
                    free(processed);
                    exit(1);
                }
                processed = temp;
            }
            processed[len++] = tolower(c);
            prev_space = 0;
        }
        else
        {
            if (!prev_space)
            {
                if (len + 1 >= size)
                {
                    size *= 2;
                    char *temp = realloc(processed, size);
                    if (!temp)
                    {
                        perror("realloc");
                        free(processed);
                        exit(1);
                    }
                    processed = temp;
                }
                processed[len++] = ' ';
                prev_space = 1;
            }
        }
    }

    if (len > 0 && processed[len - 1] == ' ')
    {
        len--;
    }
    processed[len] = '\0';
    return processed;
}


void generateNewText(char *new_sentence)
{
    char *processed = process_string(new_sentence);
    int original_count;
    char **original_words = split_into_words(processed, &original_count);
    free(processed);

    char **result_words = NULL;
    int result_count = 0;

    for (int i = 0; i < original_count; i++)
    {
        result_words = realloc(result_words, (result_count + 1) * sizeof(char *));
        result_words[result_count] = strdup(original_words[i]);
        result_count++;

        if (i == original_count - 1)
            break;

        const char *word1 = original_words[i];
        const char *word2 = original_words[i + 1];
        int word1_idx = find_index(word1);
        int word2_idx = find_index(word2);

        if (word1_idx == -1 || word2_idx == -1)
            continue;

        int num_bridges = 0;
        char **bridges = NULL;
        for (int j = 0; j < unique_count; j++)
        {
            if (matrix[word1_idx][j] > 0 && matrix[j][word2_idx] > 0)
            {
                bridges = realloc(bridges, (num_bridges + 1) * sizeof(char *));
                bridges[num_bridges++] = strdup(unique[j]);
            }
        }

        if (num_bridges > 0)
        {
            int random_idx = rand() % num_bridges;
            result_words = realloc(result_words, (result_count + 1) * sizeof(char *));
            result_words[result_count++] = bridges[random_idx];
            for (int j = 0; j < num_bridges; j++)
            {
                if (j != random_idx)
                    free(bridges[j]);
            }
            free(bridges);
        }
        else
        {
            free(bridges);
        }
    }

    // 构建结果字符串
    size_t total_len = 0;
    for (int i = 0; i < result_count; i++)
        total_len += strlen(result_words[i]) + 1;
    char *new_text = malloc(total_len);
    int pos = 0;
    for (int i = 0; i < result_count; i++)
    {
        strcpy(new_text + pos, result_words[i]);
        pos += strlen(result_words[i]);
        if (i != result_count - 1)
            new_text[pos++] = ' ';
    }
    new_text[pos] = '\0';
    printf("New text: %s\n", new_text);
    free(new_text);

    // 清理内存
    for (int i = 0; i < result_count; i++)
        free(result_words[i]);
    free(result_words);
    for (int i = 0; i < original_count; i++)
        free(original_words[i]);
    free(original_words);
}

void calcShortestPath()
{
    char word1[100], word2[100];
    printf("Enter two words: ");
    scanf("%s %s", word1, word2);

    // 处理第一个单词
    char *processed1 = process_string(word1);
    int count1;
    char **words1 = split_into_words(processed1, &count1);
    if (count1 == 0)
    {
        printf("The first word is invalid.\n");
        free(processed1);
        return;
    }
    int start = find_index(words1[0]);
    free(processed1);
    for (int i = 0; i < count1; i++)
    {
        free(words1[i]);
    }
    free(words1);

    // 处理第二个单词
    char *processed2 = process_string(word2);
    int count2;
    char **words2 = split_into_words(processed2, &count2);
    if (count2 == 0)
    {
        printf("The second word is invalid.\n");
        free(processed2);
        return;
    }
    int end = find_index(words2[0]);
    free(processed2);
    for (int i = 0; i < count2; i++)
    {
        free(words2[i]);
    }
    free(words2);

    if (start == -1 || end == -1)
    {
        printf("One or both words are not present in the graph.\n");
        return;
    }

    // 初始化Dijkstra算法所需的数据结构
    int *dist = (int *)malloc(unique_count * sizeof(int));
    int *predecessor = (int *)malloc(unique_count * sizeof(int));
    int *visited = (int *)calloc(unique_count, sizeof(int));

    for (int i = 0; i < unique_count; i++)
    {
        dist[i] = INT_MAX;
        predecessor[i] = -1;
    }
    dist[start] = 0;

    // 执行Dijkstra算法
    for (int i = 0; i < unique_count; i++)
    {
        // 找到当前未访问的最小距离节点
        int u = -1;
        int min_dist = INT_MAX;
        for (int j = 0; j < unique_count; j++)
        {
            if (!visited[j] && dist[j] < min_dist)
            {
                u = j;
                min_dist = dist[j];
            }
        }
        if (u == -1)
            break; // 所有可达节点已处理
        visited[u] = 1;

        // 更新邻接节点的距离
        for (int v = 0; v < unique_count; v++)
        {
            if (matrix[u][v] > 0)
            { // 存在边u->v
                int weight = matrix[u][v];
                if (dist[v] > dist[u] + weight)
                {
                    dist[v] = dist[u] + weight;
                    predecessor[v] = u;
                }
            }
        }
    }

    // 检查是否存在路径
    if (dist[end] == INT_MAX)
    {
        printf("No path from \"%s\" to \"%s\".\n", word1, word2);
    }
    else
    {
        // 重建路径
        int current = end;
        int path[unique_count];
        int path_len = 0;
        while (current != -1)
        {
            path[path_len++] = current;
            current = predecessor[current];
        }
        // 检查路径是否有效
        if (path[path_len - 1] != start)
        {
            printf("No path from \"%s\" to \"%s\".\n", word1, word2);
        }
        else
        {
            printf("Shortest path: ");
            for (int i = path_len - 1; i >= 0; i--)
            {
                printf("%s", unique[path[i]]);
                if (i > 0)
                {
                    printf("->");
                }
            }
            printf(", Length: %d\n", dist[end]);
        }
    }

    // 释放内存
    free(dist);
    free(predecessor);
    free(visited);
}

void calPageRank(const char *input)
{
    // 将输入单词转为小写
    char *word = strdup(input);
    for (int i = 0; word[i]; i++)
        word[i] = tolower(word[i]);

    // 查找该单词在图中的索引
    int idx = find_index(word);
    free(word);

    if (idx == -1)
    {
        printf("The word \"%s\" is not present in the graph.\n", input);
        return;
    }

    // PageRank参数初始化
    const double d = 0.85;          // 阻尼因子
    const double epsilon = 1e-6;    // 精度阈值
    const int max_iterations = 100; // 最大迭代次数
    int N = unique_count;

    // 初始化PR值
    double *PR = (double *)malloc(N * sizeof(double));
    double *new_PR = (double *)malloc(N * sizeof(double));
    for (int i = 0; i < N; i++)
    {
        PR[i] = 1.0 / N; // 初始PR值相等
    }

    // 迭代计算PageRank
    for (int iter = 0; iter < max_iterations; iter++)
    {
        double diff = 0.0;

        // 计算新的PR值
        for (int i = 0; i < N; i++)
        {
            new_PR[i] = (1.0 - d) / N; // (1 - d) / N 部分

            // 累加所有指向i的节点的PR贡献
            for (int j = 0; j < N; j++)
            {
                if (matrix[j][i] > 0) // 如果存在边 j -> i
                {
                    int out_degree_j = 0;
                    for (int k = 0; k < N; k++)
                    {
                        if (matrix[j][k] > 0) // 计算节点j的出度
                            out_degree_j++;
                    }
                    // 加上 PR(j) / out_degree_j 的贡献
                    new_PR[i] += d * PR[j] / out_degree_j;
                }
            }
        }

        // 计算PR值变化的差异
        for (int i = 0; i < N; i++)
        {
            diff += fabs(new_PR[i] - PR[i]);
            PR[i] = new_PR[i];
        }

        // 如果变化小于阈值，则提前停止
        if (diff < epsilon)
        {
            printf("PageRank computation converged in %d iterations.\n", iter + 1);
            break;
        }
    }

    // 输出目标节点的PageRank值
    printf("PageRank for \"%s\" (index %d) is: %.6f\n", input, idx, PR[idx]);

    // 清理内存
    free(PR);
    free(new_PR);
}

void randomWalk()
{
    srand(time(NULL));

    int current_node = rand() % unique_count;
    
    FILE *file = fopen("random_walk_output.txt", "w");
    if (!file)
    {
        perror("Error opening file for random walk output");
        return;
    }

    printf("%s", unique[current_node]); // Print first node
    fprintf(file, "%s", unique[current_node]);

    int visited[unique_count];
    memset(visited, 0, sizeof(visited));

    while (1)
    {
        visited[current_node] = 1;

        int next_node = -1;
        int num_out_edges = 0;
        for (int i = 0; i < unique_count; i++)
        {
            if (matrix[current_node][i] > 0)
            {
                num_out_edges++;
                if (rand() % num_out_edges == 0)
                    next_node = i;
            }
        }

        if (next_node == -1 || visited[next_node])
            break;

        printf(" %s", unique[next_node]); // Print next node with space
        fprintf(file, " %s", unique[next_node]);

        current_node = next_node;

        if (kbhit())
        {
            char ch = getch();
            if (ch != 0)
            {
                printf("\nRandom walk stopped by user input.\n");
                break;
            }
        }
    }

    printf("\n"); // Add newline at the end
    fclose(file);
    printf("\nRandom walk finished. Results saved to 'random_walk_output.txt'.\n");
}

int main()
{
    FILE *file = fopen("Cursed Be The Treasure.txt", "r"); // Easy Test        Cursed Be The Treasure
    if (!file)
    {
        perror("fopen");
        return 1;
    }

    char *processed = process_input(file);
    fclose(file);

    int word_count;
    char **words = split_into_words(processed, &word_count);
    free(processed);

    if (word_count < 2)
    {
        printf("Not enough words to form edges.\n");
        for (int i = 0; i < word_count; i++)
            free(words[i]);
        free(words);
        return 0;
    }

    
    unique = get_unique_words(words, word_count, &unique_count);

    matrix = malloc(unique_count * sizeof(int *));
    for (int i = 0; i < unique_count; i++)
        matrix[i] = calloc(unique_count, sizeof(int));

    for (int i = 0; i < word_count - 1; i++)
    {
        int current_idx = find_index(words[i]);
        int next_idx = find_index(words[i + 1]);
        if (current_idx != -1 && next_idx != -1)
        {
            matrix[current_idx][next_idx]++;
        }
    }

    int max_value = 0;
    for (int i = 0; i < unique_count; i++)
    {
        for (int j = 0; j < unique_count; j++)
        {
            if (matrix[i][j] > max_value)
                max_value = matrix[i][j];
        }
    }

    while(1)
    {
        char input[100];
        printf("\n1. Show directed graph\n");
        printf("2. Query bridge words\n");
        printf("3. generate New Text\n");
        printf("4. calculate Shortest Path\n");
        printf("5. calculate Page Rank\n");
        printf("6. random Walk\n");
        printf("7. exit\n");
        printf("choose the function number:\n");
        int choice;
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
        printf("\n");
            showDirectedGraph();
            break;
        case 2: 
            printf("\nEnter two words to find: ");
            scanf("%s", input);
            if (strcmp(input, "exit") == 0)
                break;
            char *word1 = strdup(input);
            scanf("%s", input);
            char *word2 = strdup(input);
            queryBridgeWords(word1, word2);
            free(word1);
            free(word2);
            break;
        case 3:
            printf("\nEnter a new sentence: ");
            char new_sentence[1024];
            scanf(" %1023[^\n]", new_sentence); // 读取整行输入
            generateNewText(new_sentence);
            break;
        case 4:
            printf("\nEnter two words to find the shortest path: ");
            calcShortestPath();
            break;
        case 5:
            printf("\nEnter a word to calculate PageRank: ");
            scanf("%s", input);
            calPageRank(input);
            break;
        case 6:
            printf("\n");
            randomWalk();
            break;
        case 7:
            for (int i = 0; i < unique_count; i++)
                free(matrix[i]);
            free(matrix);
            for (int i = 0; i < unique_count; i++)
                free(unique[i]);
            free(unique);
            for (int i = 0; i < word_count; i++)
                free(words[i]);
            free(words);
            return 0;
        }
        printf("input enter to continue...\n");
        getchar(); // 清除换行符
        getchar(); // 等待用户输入
    }

    for (int i = 0; i < unique_count; i++)
        free(matrix[i]);
    free(matrix);
    for (int i = 0; i < unique_count; i++)
        free(unique[i]);
    free(unique);
    for (int i = 0; i < word_count; i++)
        free(words[i]);
    free(words);

    return 0;
}