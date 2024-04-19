<script setup>
import { reactive, ref, watch } from 'vue';
import { ElInput, ElTree } from 'element-plus'

const textarea = ref('');

const data = ref([])

let lineNum = 0;
const parseTree = (text, level) => {
  const result = [];
  while(lineNum < text.length) {
    const line = text[lineNum];
    const nextLevel = line.indexOf(line.trim());
    console.log(lineNum, line, nextLevel, level)
    if (nextLevel === level) {
      result.push({ label: line.trim() });
      lineNum++;
    } else if (nextLevel > level) {
      result[result.length - 1].children = parseTree(text, nextLevel);
    } else {
      break;
    }
  }
  return result;
}

const convertAST = () => {
  lineNum = 0;
  const lines = textarea.value.trim().split('\n');
  const tree = parseTree(lines, 0);
  console.log(tree);
  data.value = tree;
}

const defaultProps = {
  children: 'children',
  label: 'label',
}
</script>

<template>
  <el-input
    v-model="textarea"
    @change="convertAST"
    :autosize="{ minRows: 2, maxRows: 32}"
    type="textarea"
    placeholder="Please input"
  />
  <el-tree
    style="width: 60vw"
    :data="data"
    :props="defaultProps"
    default-expand-all
  />
</template>

<style scoped>

</style>
