setwd("~/Symbulation/SymbulationEmp/results-0.9")
initial_data <- read.table("munged_basic.dat", h=T)
ggplot(data=initial_data, aes(x=update, y=coop, group=treatment, colour=treatment)) + ylab("Mean Cooperation Value") + xlab("Updates") + stat_summary(aes(color=factor(treatment), fill=factor(treatment)),fun.data="mean_cl_boot", geom=c("smooth"), se=TRUE) + theme(panel.background = element_rect(fill='white', colour='black')) + theme(panel.grid.major = element_blank(), panel.grid.minor = element_blank()) + guides(fill=FALSE) +scale_colour_manual(values=colors) + scale_fill_manual(values=colors) +xlim(0,1000) + ylim(-1,1)
update_1 <- subset(initial_data, update==1000)
ggplot(data=update_1, aes(x=as.factor(treatment), y=coop, group=as.factor(treatment))) + ylab("Cooperation Value") + xlab ("Mutation Rate") + geom_boxplot()
