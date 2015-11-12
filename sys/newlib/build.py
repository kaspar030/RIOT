Module().uses(ctx.CPU)\
        .needs("uart_stdio")\
        .add_includes("sys/libc/include")\
        .context.LINKFLAGS += "-specs=nano.specs -lc -lnosys"
