set pagination off
set print repeats 0
set print pretty
set logging on ./rename.log

b io_stats_rename
commands
        p *oldloc
        p *newloc
        bt
        c
end

b md_rename
commands
        p *oldloc
        p *newloc
        bt
        c
end

b ob_rename
commands
        p *oldloc
        p *newloc
        bt
        c
end

b dht_rename
commands
        p *oldloc
        p *newloc
        bt
        c
end

b ec_rename
commands
        p *oldloc
        p *newloc
        bt
        c
end

b client_rename
commands
        p *oldloc
        p *newloc
        bt
        c
end

b io_stats_rename
commands
        p *oldloc
        p *newloc
        bt
        c
end

b ro_rename
commands
        p *oldloc
        p *newloc
        bt
        c
end

b quota_rename
commands
        p *oldloc
        p *newloc
        bt
        c
end

b barrier_rename
commands
        p *oldloc
        p *newloc
        bt
        c
end

b marker_rename
commands
        p *oldloc
        p *newloc
        bt
        c
end

b iot_rename
commands
        p *oldloc
        p *newloc
        bt
        c
end

b up_rename
commands
        p *oldloc
        p *newloc
        bt
        c
end

b changelog_rename
commands
        p *oldloc
        p *newloc
        bt
        c
end

b ctr_rename
commands
        p *oldloc
        p *newloc
        bt
        c
end

b trash_rename
commands
        p *oldloc
        p *newloc
        bt
        c
end

b posix_rename
commands
        p *oldloc
        p *newloc
        bt
        c
end
