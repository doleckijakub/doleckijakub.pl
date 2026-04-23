"use client";

import { useEffect, useState } from "react";
import Section from "../ui/Section";
import CalendarHeatmap from "react-calendar-heatmap";
import "react-calendar-heatmap/dist/styles.css";

import {
    LineChart,
    Line,
    XAxis,
    YAxis,
    Tooltip,
    ResponsiveContainer,
} from "recharts";

function Stat({ label, value }: any) {
    return (
        <div className="border border-white/10 p-3 rounded">
            <div className="text-white font-mono">{value}</div>
            <div className="text-white/50 text-xs">{label}</div>
        </div>
    );
}

function formatYears(date: string) {
    const years =
        (Date.now() - new Date(date).getTime()) /
        (1000 * 60 * 60 * 24 * 365);
    return `${Math.floor(years)} years ago`;
}

function buildDailyData(weeks: any[]) {
    return weeks.flatMap((w: any) =>
        w.contributionDays.map((d: any) => ({
            date: d.date,
            count: d.contributionCount,
        }))
    );
}

function calculateStreak(days: { date: string; count: number }[]) {
    let streak = 0;

    for (let i = days.length - 1; i >= 0; i--) {
        if (days[i].count > 0) streak++;
        else break;
    }

    return streak;
}

export default function GithubStats() {
    const [data, setData] = useState<any>(null);

    useEffect(() => {
        fetch("/api/github/stats")
            .then((r) => r.json())
            .then(setData);
    }, []);

    if (!data) return <Section title="github">loading...</Section>;

    const user = data.data.user;
    const weeks = user.contributionsCollection.contributionCalendar.weeks;

    const daily = buildDailyData(weeks);
    const streak = calculateStreak(daily);

    const heatmapData = daily.map((d) => ({
        date: d.date,
        count: d.count,
    }));

    return (
        <Section title="github">
            <div className="space-y-6">

                {/* CONTRIBUTION GRAPH */}
                <div className="p-4 border border-white/10 rounded">
                    <div className="text-xs text-white/50 mb-2">
                        contributions (last 365 days)
                    </div>

                    <ResponsiveContainer width="100%" height={200}>
                        <LineChart data={daily}>
                            <XAxis dataKey="date" hide />
                            <YAxis hide />
                            <Tooltip />
                            <Line
                                type="monotone"
                                dataKey="count"
                                stroke="#22d3ee"
                                dot={false}
                            />
                        </LineChart>
                    </ResponsiveContainer>
                </div>

                {/* stats row */}
                <div className="grid grid-cols-2 sm:grid-cols-4 gap-4 text-sm">
                    <Stat
                        label="total contributions"
                        value={
                            user.contributionsCollection.contributionCalendar
                                .totalContributions
                        }
                    />
                    <Stat
                        label="public repos"
                        value={user.repositories.totalCount}
                    />
                    <Stat
                        label="joined"
                        value={formatYears(user.createdAt)}
                    />
                    <Stat label="current streak" value={`${streak} days`} />
                </div>

                {/* HEATMAP */}
                <div className="p-4 border border-white/10 rounded">
                    <CalendarHeatmap
                        startDate={
                            new Date(Date.now() - 365 * 24 * 60 * 60 * 1000)
                        }
                        endDate={new Date()}
                        values={heatmapData}
                        classForValue={(v: any) => {
                            if (!v) return "color-empty";
                            return `color-scale-${Math.min(v.count, 4)}`;
                        }}
                    />
                </div>

            </div>
        </Section>
    );
}