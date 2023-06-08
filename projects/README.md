## 模板说明

### template
- 采用 app + lib 模式

## 依赖

### qtcanpool

模板采用了 [qtcanpool](https://gitee.com/icanpool/qtcanpool) 工程模板，所以需要依赖 qtcanpool 的工程管理。

通过在模板目录下的 config.pri 文件中配置 QTCANPOOL_DIR 变量来关联 qtcanpool 工程，如：
```
isEmpty(QTCANPOOL_DIR):  QTCANPOOL_DIR = $$quote(D:\projects\qt\qtcanpool)
```
此处说明 qtcanpool 工程放在 D 盘的 projects\qt 目录中。

备注：推荐使用 qtcanpool 的 master 分支。
