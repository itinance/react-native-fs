import React from 'react';
import { StyleSheet, Text, View } from 'react-native';

export default function TestCase({ details, name, status }: PropsT) {
  const [statusState, setStatusState] = React.useState<Status>(
    typeof status === 'string' ? status : 'wait',
  );

  React.useEffect(() => {
    if (typeof status === 'string') {
      setStatusState(status);
    } else {
      (async () => {
        setStatusState('wait');
        const res = await status();
        setStatusState(res);
      })();
    }
  }, [status]);

  return (
    <View style={[styles.container, styles[statusState]]}>
      <Text style={styles.name}>{name}</Text>
      {details && <Text>{details}</Text>}
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    borderBottomWidth: 1,
  },
  name: {
    fontWeight: 'bold',
  },
  fail: {
    backgroundColor: 'red',
  },
  pass: {
    backgroundColor: 'limegreen',
  },
  wait: {
    backgroundColor: 'yellow',
  },
});

export type Status = 'fail' | 'pass' | 'wait';

export type StatusOrEvaluator =
  | Status
  | (() => Status)
  | (() => Promise<Status>);

type PropsT = {
  name: string;
  details?: string;
  status: StatusOrEvaluator;
};
